#include <glib.h>
#include <libeda/eda.h>
#include "signaltracker.h"

static void
test_pin_basic()
{
    EdaObject *pin;
    gint x1, y1, x2, y2;

    pin = eda_pin_new(EDA_PIN_KIND_NET, 0, 10, 10, 20, 10, 3);
    g_assert(pin != NULL);
    g_assert(eda_object_get_page(pin) == NULL);

    g_assert_cmpint(eda_pin_get_color(EDA_PIN(pin)), ==, 3);
    eda_pin_set_color(EDA_PIN(pin), 6);
    g_assert_cmpint(eda_pin_get_color(EDA_PIN(pin)), ==, 6);

    eda_pin_get_coords(EDA_PIN(pin), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 10);
    g_assert_cmpint(y1, ==, 10);
    g_assert_cmpint(x2, ==, 20);
    g_assert_cmpint(y2, ==, 10);

    eda_pin_set_coords(EDA_PIN(pin), 2, 3, 4, 5);
    eda_pin_get_coords(EDA_PIN(pin), &x1, &y1, &x2, &y2);
    g_assert_cmpint(x1, ==, 2);
    g_assert_cmpint(y1, ==, 3);
    g_assert_cmpint(x2, ==, 4);
    g_assert_cmpint(y2, ==, 5);

    eda_pin_get_position(EDA_PIN(pin), &x2, &y2);
    g_assert_cmpint(x2, ==, 2);
    g_assert_cmpint(y2, ==, 3);

    g_object_add_weak_pointer(G_OBJECT(pin), (gpointer) &pin);
    g_object_unref(pin);
    g_assert(pin == NULL);
}

static void
test_pin_signals()
{
    EdaPage *page;
    EdaObject *pin;
    SignalTracker *st;

    pin = eda_pin_new(EDA_PIN_KIND_NET, 0, 10, 10, 20, 10, 3);

    st = signal_tracker_new();

    signal_tracker_track(st, pin, NULL);

    eda_pin_set_coords(EDA_PIN(pin), 1, 2, 3, 4);
    g_assert(signal_tracker_match(st, pin, "pre-changed"));
    g_assert(signal_tracker_match(st, pin, "changed"));
    g_assert(signal_tracker_is_empty(st));

    eda_pin_set_color(EDA_PIN(pin), 1);
    g_assert(signal_tracker_match(st, pin, "pre-changed"));
    g_assert(signal_tracker_match(st, pin, "changed"));
    g_assert(signal_tracker_is_empty(st));

    /* test signal emission on a page */
    page = eda_page_new();
    eda_page_add_object(page, pin);

    signal_tracker_track(st, page, NULL);

    eda_object_translate(pin, 12, 34);
    g_assert(signal_tracker_match(st, pin, "pre-changed"));
    g_assert(signal_tracker_match(st, page, "pre-object-changed"));
    g_assert(signal_tracker_match(st, pin, "translate"));
    g_assert(signal_tracker_match(st, pin, "changed"));
    g_assert(signal_tracker_match(st, page, "object-changed"));
    g_assert(signal_tracker_is_empty(st));

    signal_tracker_free(st);
    g_object_unref(page);
}

static void
test_pin_copy()
{
    EdaObject *pin, *copy;
    gint x1, y1, x2, y2;
    gint cx1, cy1, cx2, cy2;

    pin = eda_pin_new(EDA_PIN_KIND_NET, 0, 1, 2, 3, 4, 5);

    copy = eda_object_copy(pin);
    g_assert(copy != NULL);

    eda_pin_get_coords(EDA_PIN(pin), &x1, &y1, &x2, &y2);
    eda_pin_get_coords(EDA_PIN(copy), &cx1, &cy1, &cx2, &cy2);
    g_assert_cmpint(x1, ==, cx1);
    g_assert_cmpint(y1, ==, cy1);
    g_assert_cmpint(x2, ==, cx2);
    g_assert_cmpint(y2, ==, cy2);
    g_assert_cmpint(eda_pin_get_color(EDA_PIN(pin)), ==,
                    eda_pin_get_color(EDA_PIN(copy)));

    g_object_unref(pin);
    g_object_unref(copy);
}

int main(int argc, char **argv)
{
    g_type_init();
    g_test_init(&argc, &argv, NULL);

    eda_types_init();

    g_test_add_func("/pin/basic", test_pin_basic);
    g_test_add_func("/pin/copy", test_pin_copy);
    g_test_add_func("/pin/signals", test_pin_signals);
    return g_test_run();
}


