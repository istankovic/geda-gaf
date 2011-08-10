#include <glib.h>
#include <glib-object.h>
#include "signaltracker.h"

typedef struct
{
    gchar *name;
    gpointer instance;
} Emission;

struct _SignalTracker
{
    GList *hooks;
    GList *signal_ids;
    GQueue *emissions;
};

static Emission *
emission_new(const gchar *name, gpointer instance)
{
    Emission *e = g_new(Emission, 1);

    e->name = g_strdup(name);
    e->instance = instance;
    return e;
}

static void
emission_free(Emission *e)
{
    g_free(e->name);
    g_free(e);
}

static gboolean
signal_emission_hook(GSignalInvocationHint *ihint, guint n_param_values,
                     const GValue *param_values, gpointer data)
{
    Emission *e;
    gpointer instance;
    SignalTracker *st = (SignalTracker *) data;

    instance = g_value_get_object(&param_values[0]);
    e = emission_new(g_signal_name(ihint->signal_id), instance);
    g_queue_push_head(st->emissions, e);
    return TRUE;
}

SignalTracker *
signal_tracker_new()
{
    SignalTracker *st;

    st = g_new(SignalTracker, 1);
    st->hooks = NULL;
    st->signal_ids = NULL;
    st->emissions = g_queue_new();
    return st;
}

static void
add_emmision_hook(SignalTracker *st, gpointer instance, guint signal_id)
{
    gulong hook_id;
    GSignalQuery query;

    g_assert(signal_id != 0);

    if(g_list_find(st->signal_ids, GUINT_TO_POINTER(signal_id))) {
        g_debug("attempt to track already tracked signal %s::%s\n",
                g_type_name_from_instance(instance),
                g_signal_name(signal_id));
        return;
    }

    /* silently skip over signals having G_SIGNAL_NO_HOOKS flag */
    g_signal_query(signal_id, &query);
    if(query.signal_flags & G_SIGNAL_NO_HOOKS)
        return;

    hook_id = g_signal_add_emission_hook(signal_id, 0,
                                         signal_emission_hook, st, NULL);
    st->hooks = g_list_prepend(st->hooks, (gpointer) hook_id);
    st->signal_ids = g_list_prepend(st->signal_ids, GUINT_TO_POINTER(signal_id));
}

static GList *
get_all_signals_for_type(GType type)
{
    GList *lst = NULL;
    guint i, n_ids, *ids;

    while(type != 0) {
        ids = g_signal_list_ids(type, &n_ids);
        for(i = 0; i < n_ids; i++)
            lst = g_list_prepend(lst, GUINT_TO_POINTER(ids[i]));
        g_free(ids);
        type = g_type_parent(type);
    }
    return lst;
}

void
signal_tracker_track(SignalTracker *st, gpointer instance, const gchar *name)
{
    GType type;
    GList *lst;

    type = G_TYPE_FROM_INSTANCE(instance);
    g_assert(type != G_TYPE_INVALID);

    if(name) {
        add_emmision_hook(st, instance, g_signal_lookup(name, type));
    } else {
        /* add hooks for all signals */
        lst = get_all_signals_for_type(type);
        while(lst) {
            add_emmision_hook(st, instance, GPOINTER_TO_UINT(lst->data));
            lst = g_list_delete_link(lst, lst);
        }
    }
}

gboolean
signal_tracker_is_empty(SignalTracker *st)
{
    Emission *e;

    if(!g_queue_is_empty(st->emissions)) {
        e = (Emission *) g_queue_pop_tail(st->emissions);
        g_debug("unexpected signal emission: [%s %p]::%s\n",
                g_type_name_from_instance(e->instance), e->instance, e->name);
        return FALSE;
    }
    return TRUE;
}

gboolean
signal_tracker_match(SignalTracker *st, gpointer instance, const gchar *name)
{
    Emission *e;
    gboolean match;

    if(g_queue_is_empty(st->emissions)) {
        g_debug("expected signal emission: [%s %p]::%s, but the tracker is empty\n",
                g_type_name_from_instance(instance), instance, name);
        return FALSE;
    }

    e = g_queue_pop_tail(st->emissions);

    match = instance == e->instance && strcmp(name, e->name) == 0;
    if(!match) {
        g_debug("expected signal emission: [%s %p]::%s, got [%s %p]::%s\n",
                g_type_name_from_instance(instance), instance, name,
                g_type_name_from_instance(e->instance), e->instance, e->name);
    }

    emission_free(e);
    return match;
}

void
signal_tracker_free(SignalTracker *st)
{
    Emission *e;
    gulong hook_id;
    guint signal_id;

    while(!g_queue_is_empty(st->emissions)) {
        e = (Emission *) g_queue_pop_head(st->emissions);
        emission_free(e);
    }

    st->emissions = NULL;

    while(st->hooks) {
        hook_id = (gulong) st->hooks->data;
        signal_id = GPOINTER_TO_UINT(st->signal_ids->data);

        g_signal_remove_emission_hook(signal_id, hook_id);

        st->hooks = g_list_delete_link(st->hooks, st->hooks);
        st->signal_ids = g_list_delete_link(st->signal_ids, st->signal_ids);
    }
}

