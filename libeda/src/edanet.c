#include <stdio.h>

#include <libeda/edanet.h>
#include <libeda/edaconfig.h>
#include <libeda/edaerrors.h>
#include "i18n.h"
#include "util.h"

#define EDA_NET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EDA_TYPE_NET, EdaNetPrivate))

G_DEFINE_TYPE (EdaNet, eda_net, EDA_TYPE_OBJECT);

struct _EdaNetPrivate
{
    gint color;
    gint x1, y1;
    gint x2, y2;
};

static EdaObject *
eda_net_copy(EdaObject *obj)
{
    EdaObject *copy;
    EdaNetPrivate *priv, *orig_priv;

    copy = EDA_OBJECT_CLASS(eda_net_parent_class)->copy(obj);

    priv = EDA_NET(copy)->priv;
    orig_priv = EDA_NET(obj)->priv;

    priv->x1 = orig_priv->x1;
    priv->y1 = orig_priv->y1;
    priv->x2 = orig_priv->x2;
    priv->y2 = orig_priv->y2;
    priv->color = orig_priv->color;
    return copy;
}

static gchar *
eda_net_to_string(EdaObject *obj)
{
    EdaNetPrivate *priv = EDA_NET(obj)->priv;

    return g_strdup_printf("N %d %d %d %d %d\n",
                           priv->x1, priv->y1,
                           priv->x2, priv->y2, priv->color);
}

EdaObject *
eda_net_from_string(const gchar *buf, size_t size, size_t *offset, GError **err)
{
    gint color;
    gint x1, y1;
    gint x2, y2;
    int items, consumed;

    items = sscanf(buf + *offset, "N %d %d %d %d %d\n%n",
                   &x1, &y1, &x2, &y2, &color, &consumed);
    if (items != 5) {
        g_set_error(err, EDA_ERROR, EDA_ERROR_DESERIALIZATION,
                    _("invalid net object at offset %zu"), *offset);
        return NULL;
    }

    *offset += consumed;
    return eda_net_new(color, x1, y1, x2, y2);
}

static void
eda_net_get_bounds(EdaObject *obj, gint *left, gint *top,
                   gint *right, gint *bottom)
{
    gint halfwidth;
    EdaConfig *cfg;
    EdaNetPrivate *priv = EDA_NET(obj)->priv;

    *left = MIN(priv->x1, priv->x2);
    *top = MIN(priv->y1, priv->y2);
    *right = MAX(priv->x1, priv->x2);
    *bottom = MAX(priv->y1, priv->y2);

    cfg = eda_object_get_config(obj);
    halfwidth = eda_config_get_int(cfg, "graphical", "net-width") / 2;

    /* This isn't strictly correct, but a 1st order approximation */
    *left   -= halfwidth;
    *top    -= halfwidth;
    *right  += halfwidth;
    *bottom += halfwidth;
}

static void
eda_net_translate(EdaObject *obj, gint dx, gint dy)
{
    EdaNetPrivate *priv = EDA_NET(obj)->priv;

    priv->x1 += dx;
    priv->y1 += dy;
    priv->x2 += dx;
    priv->y2 += dy;
}

static void
eda_net_mirror(EdaObject *obj, gint x, gint y)
{
    EdaNetPrivate *priv = EDA_NET(obj)->priv;

    /* translate object to origin */
    eda_net_translate(obj, -x, -y);

    priv->x1 = -priv->x1;
    priv->x2 = -priv->x2;

    eda_net_translate(obj, x, y);
}

static void
eda_net_rotate(EdaObject *obj, gint x, gint y, gint angle)
{
    EdaNetPrivate *priv = EDA_NET(obj)->priv;

    /* translate object to origin */
    eda_net_translate(obj, -x, -y);

    rotate_point_90(&priv->x1, &priv->y1, angle);
    rotate_point_90(&priv->x2, &priv->y2, angle);

    eda_net_translate(obj, x, y);
}

static void
eda_net_class_init(EdaNetClass *klass)
{
    EdaObjectClass *o_klass = EDA_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(EdaNetPrivate));

    o_klass->copy = eda_net_copy;
    o_klass->to_string = eda_net_to_string;
    o_klass->from_string = eda_net_from_string;

    o_klass->mirror = eda_net_mirror;
    o_klass->rotate = eda_net_rotate;
    o_klass->translate = eda_net_translate;
    o_klass->get_bounds = eda_net_get_bounds;
}

static void
eda_net_init(EdaNet *page)
{
    EdaNetPrivate *priv;

    page->priv = priv = EDA_NET_GET_PRIVATE(page);
    memset(priv, 0, sizeof(*priv));
}

/**
 * eda_net_new:
 * @color: the color for the net
 * @x1: the x1 coordinate
 * @y1: the y1 coordinate
 * @x2: the x2 coordinate
 * @y2: the y2 coordinate
 *
 * Returns: an #EdaNet object
 */
EdaObject *
eda_net_new(gint color, gint x1, gint y1, gint x2, gint y2)
{
    EdaNetPrivate *priv;
    EdaObject *net = g_object_new(EDA_TYPE_NET, NULL);

    priv = EDA_NET(net)->priv;
    priv->color = color;
    priv->x1 = x1;
    priv->y1 = y1;
    priv->x2 = x2;
    priv->y2 = y2;
    return net;
}

/**
 * eda_net_get_color:
 * @net: an #EdaNet
 *
 * Returns: the net's color
 */
gint
eda_net_get_color(EdaNet *net)
{
    g_return_val_if_fail(EDA_IS_NET(net), 0);

    return net->priv->color;
}

/**
 * eda_net_set_color:
 * @net: an #EdaNet
 * @color: the new color
 *
 * Sets the @net's color.
 */
void
eda_net_set_color(EdaNet *net, gint color)
{
    g_return_if_fail(EDA_IS_NET(net));

    eda_object_emit_pre_changed(EDA_OBJECT(net));
    net->priv->color = color;
    eda_object_emit_changed(EDA_OBJECT(net));
}

/**
 * eda_net_get_coords:
 * @net: an #EdaNet
 * @x1: location to store the x1 coordinate
 * @y1: location to store the y1 coordinate
 * @x2: location to store the x2 coordinate
 * @y2: location to store the y2 coordinate
 *
 * Returns: the net's coordinates
 */
void
eda_net_get_coords(EdaNet *net, gint *x1, gint *y1, gint *x2, gint *y2)
{
    g_return_if_fail(EDA_IS_NET(net));

    *x1 = net->priv->x1;
    *y1 = net->priv->y1;
    *x2 = net->priv->x2;
    *y2 = net->priv->y2;
}

/**
 * eda_net_set_coords:
 * @net: an #EdaNet
 * @x1: the new x1 coordinate
 * @y1: the new y1 coordinate
 * @x2: the new x2 coordinate
 * @y2: the new y2 coordinate
 *
 * Sets the @net's coordinates.
 */
void
eda_net_set_coords(EdaNet *net, gint x1, gint y1, gint x2, gint y2)
{
    EdaNetPrivate *priv;

    g_return_if_fail(EDA_IS_NET(net));

    eda_object_emit_pre_changed(EDA_OBJECT(net));
    priv = net->priv;
    priv->x1 = x1;
    priv->y1 = y1;
    priv->x2 = x2;
    priv->y2 = y2;
    eda_object_emit_changed(EDA_OBJECT(net));
}

