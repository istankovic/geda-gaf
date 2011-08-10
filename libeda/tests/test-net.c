#include <glib.h>
#include <libeda/eda.h>
#include "signaltracker.h"

static void
test_net_basic()
{
    EdaObject *net;
    gint x1, y1, x2, y2;

    net = eda_net_new(1, 9, 8, 7, 6);
    g_assert(net != NULL);
    g_assert(eda_object_get_page(net) == NULL);

    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 9);
    g_assert_cmpint(y1, ==, 8);
    g_assert_cmpint(x2, ==, 7);
    g_assert_cmpint(y2, ==, 6);

    eda_net_set_coords(EDA_NET(net), 2, 3, 4, 5);
    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 2);
    g_assert_cmpint(y1, ==, 3);
    g_assert_cmpint(x2, ==, 4);
    g_assert_cmpint(y2, ==, 5);
    g_assert_cmpint(eda_net_get_color(EDA_NET(net)), ==, 1);

    /* mirror */
    eda_object_mirror(net, 1, 4);
    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 0);
    g_assert_cmpint(y1, ==, 3);
    g_assert_cmpint(x2, ==, -2);
    g_assert_cmpint(y2, ==, 5);

    /* return to the original state */
    eda_object_mirror(net, 1, 4);

    /* rotate */
    eda_object_rotate(net, 0, 0, 90);
    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, -3);
    g_assert_cmpint(y1, ==, 2);
    g_assert_cmpint(x2, ==, -5);
    g_assert_cmpint(y2, ==, 4);

    /* return to the original state */
    eda_object_rotate(net, 0, 0, 270);

    /* translate */
    eda_object_translate(net, 14, -7);
    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 16);
    g_assert_cmpint(y1, ==, -4);
    g_assert_cmpint(x2, ==, 18);
    g_assert_cmpint(y2, ==, -2);

    g_object_add_weak_pointer(G_OBJECT(net), (gpointer) &net);
    g_object_unref(net);
    g_assert(net == NULL);
}

static void
test_net_signals()
{
    EdaPage *page;
    EdaObject *net;
    SignalTracker *st;

    net = eda_net_new(1, 2, 3, 4, 5);

    st = signal_tracker_new();

    /* for now, just track all signals */
    signal_tracker_track(st, net, NULL);

    eda_object_mirror(net, 0, 0);
    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "mirror"));
    g_assert(signal_tracker_match(st, net, "changed"));
    g_assert(signal_tracker_is_empty(st));

    /* a signal is not emitted because rotation with a zero
     * angle is a no-op */
    eda_object_rotate(net, 0, 0, 0);
    g_assert(signal_tracker_is_empty(st));

    eda_object_rotate(net, 0, 0, 90);
    eda_object_rotate(net, 0, 0, 270);
    eda_object_translate(net, 0, 0);
    eda_object_rotate(net, 0, 0, 90);
    eda_object_mirror(net, 0, 0);
    eda_object_rotate(net, 0, 0, 270);

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "rotate"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "rotate"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "translate"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "rotate"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "mirror"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "rotate"));
    g_assert(signal_tracker_match(st, net, "changed"));

    g_assert(signal_tracker_is_empty(st));

    /* test signal emission on a page */
    page = eda_page_new();
    eda_page_add_object(page, net);

    signal_tracker_track(st, page, NULL);

    eda_object_translate(net, 12, 34);
    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, net, "translate"));
    g_assert(signal_tracker_match(st, net, "changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_rotate(net, 0, 0, 90);
    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, net, "rotate"));
    g_assert(signal_tracker_match(st, net, "changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_mirror(net, 0, 0);
    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, net, "mirror"));
    g_assert(signal_tracker_match(st, net, "changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    /* we want to check that the signals are emitted only on the net object
     * after we remove it from the page */
    g_object_ref(net);
    eda_page_remove_object(page, net);
    g_assert(signal_tracker_match(st, page, "remove-object"));
    g_assert(signal_tracker_is_empty(st));

    eda_object_translate(net, 12, 34);
    g_assert(signal_tracker_match(st, net, "pre-changed"));
    g_assert(signal_tracker_match(st, net, "translate"));
    g_assert(signal_tracker_match(st, net, "changed"));
    g_assert(signal_tracker_is_empty(st));
    g_object_unref(net);

    signal_tracker_free(st);
    g_object_unref(page);
}

static void
test_net_copy()
{
    EdaObject *net, *copy;
    gint x1, y1, x2, y2;
    gint cx1, cy1, cx2, cy2;

    net = eda_net_new(1, 2, 3, 4, 5);

    copy = eda_object_copy(net);
    g_assert(copy != NULL);

    eda_net_get_coords(EDA_NET(net), &x1, &y1, &x2, &y2);
    eda_net_get_coords(EDA_NET(copy), &cx1, &cy1, &cx2, &cy2);
    g_assert_cmpint(x1, ==, cx1);
    g_assert_cmpint(y1, ==, cy1);
    g_assert_cmpint(x2, ==, cx2);
    g_assert_cmpint(y2, ==, cy2);
    g_assert_cmpint(eda_net_get_color(EDA_NET(net)), ==,
                    eda_net_get_color(EDA_NET(copy)));

    g_object_unref(net);
    g_object_unref(copy);
}

int main(int argc, char **argv)
{
    g_type_init();
    g_test_init(&argc, &argv, NULL);

    eda_types_init();

    g_test_add_func("/net/basic", test_net_basic);
    g_test_add_func("/net/copy", test_net_copy);
    g_test_add_func("/net/signals", test_net_signals);
    return g_test_run();
}

