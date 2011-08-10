#include <glib.h>
#include <libeda/eda.h>
#include "signaltracker.h"

static void
test_page_basic()
{
    GList *lst;
    EdaPage *page;
    EdaObject *net;

    page = eda_page_new();
    g_assert(page != NULL);
    g_assert(eda_page_get_objects(page) == NULL);
    g_assert(eda_page_get_filename(page) == NULL);

    net = eda_net_new(0, 1, 2, 3, 4);
    eda_page_add_object(page, net);

    lst = eda_page_get_objects(page);
    g_assert(lst != NULL);
    g_assert(g_list_length(lst) == 1);
    g_assert(lst->data == net);
    g_list_free(lst);

    g_object_add_weak_pointer(G_OBJECT(page), (gpointer) &page);
    g_object_unref(page);
    g_assert(page == NULL);
}

static void
test_page_signals()
{
    EdaPage *page;
    EdaObject *net;
    SignalTracker *st;

    page = eda_page_new();

    st = signal_tracker_new();
    signal_tracker_track(st, page, NULL);

    net = eda_net_new(0, 1, 2, 3, 4);

    eda_page_add_object(page, net);
    g_assert(signal_tracker_match(st, page, "add-object"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_translate(net, -13, -17);
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_mirror(net, 0, 0);
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_rotate(net, 23, 41, 270);
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_page_remove_object(page, net);
    g_assert(signal_tracker_match(st, page, "remove-object"));
    g_assert(signal_tracker_is_empty(st));

    signal_tracker_free(st);
    g_object_unref(page);
}

static void
test_page_serialization()
{
    GError *err = NULL;
    EdaPage *page;
    gchar *file_contents, *page_string;

    page = eda_page_new_from_file("nonexistent-page", &err);
    g_assert(page == NULL);
    g_assert(err != NULL);
    g_clear_error(&err);

    page = eda_page_new_from_file("page.sch", &err);
    g_assert(err == NULL);
    g_assert(page != NULL);
    g_assert_cmpstr(eda_page_get_filename(page), ==, "page.sch");

    g_assert(g_file_get_contents("page.sch", &file_contents, NULL, NULL));

    page_string = eda_page_to_string(page);
    g_assert(page_string != NULL);
    g_assert_cmpstr(page_string, ==, file_contents);

    g_free(page_string);
    g_free(file_contents);
    g_object_unref(page);
}

int main(int argc, char **argv)
{
    g_type_init();
    g_test_init(&argc, &argv, NULL);

    eda_types_init();

    g_test_add_func("/page/basic", test_page_basic);
    g_test_add_func("/page/signals", test_page_signals);
    g_test_add_func("/page/serialization", test_page_serialization);
    return g_test_run();
}

