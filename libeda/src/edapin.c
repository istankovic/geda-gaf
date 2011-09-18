#include <stdio.h>

#include <libeda/edapin.h>
#include <libeda/edaconfig.h>
#include <libeda/edaerrors.h>
#include "i18n.h"
#include "util.h"

#define EDA_PIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EDA_TYPE_PIN, EdaPinPrivate))

G_DEFINE_TYPE (EdaPin, eda_pin, EDA_TYPE_OBJECT);

struct _EdaPinPrivate
{
    EdaPinKind kind;
    gint color;
    gint whichend;
    gint x1, y1;
    gint x2, y2;
};

static EdaObject *
eda_pin_copy(EdaObject *obj)
{
    EdaObject *copy;
    EdaPinPrivate *priv, *orig_priv;

    copy = EDA_OBJECT_CLASS(eda_pin_parent_class)->copy(obj);

    priv = EDA_PIN(copy)->priv;
    orig_priv = EDA_PIN(obj)->priv;

    priv->x1 = orig_priv->x1;
    priv->y1 = orig_priv->y1;
    priv->x2 = orig_priv->x2;
    priv->y2 = orig_priv->y2;
    priv->kind = orig_priv->kind;
    priv->color = orig_priv->color;
    priv->whichend = orig_priv->whichend;
    return copy;
}

static gchar *
eda_pin_to_string(EdaObject *obj)
{
    EdaPinPrivate *priv = EDA_PIN(obj)->priv;

    return g_strdup_printf("P %d %d %d %d %d %d %d",
                           priv->x1, priv->y1,
                           priv->x2, priv->y2,
                           priv->color,
                           priv->kind == EDA_PIN_KIND_NET ? 0 : 1,
                           priv->whichend);
}

EdaObject *
eda_pin_from_string(const gchar *buf, size_t size, size_t *offset, GError **err)
{
    gint kind;
    gint color;
    gint whichend;
    gint x1, y1;
    gint x2, y2;
    int items, consumed;

    items = sscanf(buf + *offset, "P %d %d %d %d %d %d %d\n%n",
                   &x1, &y1, &x2, &y2, &color, &kind, &whichend, &consumed);
    if (items != 7) {
        g_set_error(err, EDA_ERROR, EDA_ERROR_DESERIALIZATION,
                    _("invalid pin object at offset %zu"), *offset);
        return NULL;
    }

    *offset += consumed;
    return eda_pin_new(kind == 0 ? EDA_PIN_KIND_NET : EDA_PIN_KIND_BUS,
                       whichend, x1, y1, x2, y2, color);
}

static void
eda_pin_get_bounds(EdaObject *obj, gint *left, gint *top,
                   gint *right, gint *bottom)
{
    gint halfwidth;
    EdaConfig *cfg;
    EdaPinPrivate *priv = EDA_PIN(obj)->priv;

    *left = MIN(priv->x1, priv->x2);
    *top = MIN(priv->y1, priv->y2);
    *right = MAX(priv->x1, priv->x2);
    *bottom = MAX(priv->y1, priv->y2);

    cfg = eda_object_get_config(obj);
    if (priv->kind == EDA_PIN_KIND_NET)
        halfwidth = eda_config_get_int(cfg, "graphical", "pin-width-net") / 2;
    else
        halfwidth = eda_config_get_int(cfg, "graphical", "pin-width-bus") / 2;

    /* This isn't strictly correct, but a 1st order approximation */
    *left   -= halfwidth;
    *top    -= halfwidth;
    *right  += halfwidth;
    *bottom += halfwidth;
}

static void
eda_pin_translate(EdaObject *obj, gint dx, gint dy)
{
    EdaPinPrivate *priv = EDA_PIN(obj)->priv;

    priv->x1 += dx;
    priv->y1 += dy;
    priv->x2 += dx;
    priv->y2 += dy;
}

static void
eda_pin_mirror(EdaObject *obj, gint x, gint y)
{
    EdaPinPrivate *priv = EDA_PIN(obj)->priv;

    /* translate object to origin */
    eda_pin_translate(obj, -x, -y);

    priv->x1 = -priv->x1;
    priv->x2 = -priv->x2;

    eda_pin_translate(obj, x, y);
}

static void
eda_pin_rotate(EdaObject *obj, gint x, gint y, gint angle)
{
    EdaPinPrivate *priv = EDA_PIN(obj)->priv;

    /* translate object to origin */
    eda_pin_translate(obj, -x, -y);

    rotate_point_90(&priv->x1, &priv->y1, angle);
    rotate_point_90(&priv->x2, &priv->y2, angle);

    eda_pin_translate(obj, x, y);
}

static void
eda_pin_class_init(EdaPinClass *klass)
{
    EdaObjectClass *o_klass = EDA_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(EdaPinPrivate));

    o_klass->copy = eda_pin_copy;
    o_klass->to_string = eda_pin_to_string;
    o_klass->from_string = eda_pin_from_string;

    o_klass->mirror = eda_pin_mirror;
    o_klass->rotate = eda_pin_rotate;
    o_klass->translate = eda_pin_translate;
    o_klass->get_bounds = eda_pin_get_bounds;
}

static void
eda_pin_init(EdaPin *page)
{
    EdaPinPrivate *priv;

    page->priv = priv = EDA_PIN_GET_PRIVATE(page);
    memset(priv, 0, sizeof(*priv));
}

/**
 * eda_pin_new:
 * @kind: the kind of pin to construct
 * @whichend: index of the active end of the pin (0 or 1)
 * @x1: the x1 coordinate
 * @y1: the y1 coordinate
 * @x2: the x2 coordinate
 * @y2: the y2 coordinate
 * @color: the color for the pin
 *
 * Returns: an #EdaPin object
 */
EdaObject *
eda_pin_new(EdaPinKind kind, gint whichend, gint x1, gint y1, gint x2, gint y2,
            gint color)
{
    EdaPinPrivate *priv;
    EdaObject *pin = g_object_new(EDA_TYPE_PIN, NULL);

    priv = EDA_PIN(pin)->priv;
    priv->kind = kind;
    priv->whichend = whichend;
    priv->x1 = x1;
    priv->y1 = y1;
    priv->x2 = x2;
    priv->y2 = y2;
    priv->color = color;
    return pin;
}

/**
 * eda_pin_get_color:
 * @pin: an #EdaPin
 *
 * Returns: the pin's color
 */
gint
eda_pin_get_color(EdaPin *pin)
{
    g_return_val_if_fail(EDA_IS_PIN(pin), 0);

    return pin->priv->color;
}

/**
 * eda_pin_set_color:
 * @pin: an #EdaPin
 * @color: the new color
 *
 * Sets the @pin's color.
 */
void
eda_pin_set_color(EdaPin *pin, gint color)
{
    g_return_if_fail(EDA_IS_PIN(pin));

    eda_object_emit_pre_changed(EDA_OBJECT(pin));
    pin->priv->color = color;
    eda_object_emit_changed(EDA_OBJECT(pin));
}

/**
 * eda_pin_get_coords:
 * @pin: an #EdaPin
 * @x1: location to store the x1 coordinate
 * @y1: location to store the y1 coordinate
 * @x2: location to store the x2 coordinate
 * @y2: location to store the y2 coordinate
 *
 * Returns: the pin's coordinates
 */
void
eda_pin_get_coords(EdaPin *pin, gint *x1, gint *y1, gint *x2, gint *y2)
{
    g_return_if_fail(EDA_IS_PIN(pin));

    *x1 = pin->priv->x1;
    *y1 = pin->priv->y1;
    *x2 = pin->priv->x2;
    *y2 = pin->priv->y2;
}

/**
 * eda_pin_set_coords:
 * @pin: an #EdaPin
 * @x1: the new x1 coordinate
 * @y1: the new y1 coordinate
 * @x2: the new x2 coordinate
 * @y2: the new y2 coordinate
 *
 * Sets the @pin's coordinates.
 */
void
eda_pin_set_coords(EdaPin *pin, gint x1, gint y1, gint x2, gint y2)
{
    EdaPinPrivate *priv;

    g_return_if_fail(EDA_IS_PIN(pin));

    eda_object_emit_pre_changed(EDA_OBJECT(pin));
    priv = pin->priv;
    priv->x1 = x1;
    priv->y1 = y1;
    priv->x2 = x2;
    priv->y2 = y2;
    eda_object_emit_changed(EDA_OBJECT(pin));
}

/**
 * eda_pin_get_position:
 * @pin: an #EdaPin
 * @x: location to store the x coordinate
 * @y: location to store the y coordinate
 *
 * Returns: the coordinates of the connectable end (whichend) of the pin
 */
void
eda_pin_get_position(EdaPin *pin, gint *x, gint *y)
{
    g_return_if_fail(EDA_IS_PIN(pin));

    if (pin->priv->whichend == 0) {
        *x = pin->priv->x1;
        *y = pin->priv->y1;
    } else {
        *x = pin->priv->x2;
        *y = pin->priv->y2;
    }
}

