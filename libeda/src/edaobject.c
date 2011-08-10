#include <libeda/edaobject.h>
#include <libeda/edapage.h>
#include <libeda/edaerrors.h>

#include "edamarshallers.h"
#include "i18n.h"

/* declarations */
EdaObjectClass *_eda_object_class_from_char(guchar c);

/**
 * SECTION:edaobject
 * @short_description: A base, non-instantiatable type for schematic objects.
 * @title: EdaObject
 *
 * The #EdaObject class is an abstract class that all schematic objects'
 * classes should derive from. It provides minimum infrastructure needed
 * for features like copying, (de)serialization etc.
 *
 * An #EdaObject-derived object may correspond to an element in the schematic
 * such as a box, a circle, a net, an attribute etc. Each object may optionally
 * belong to a single #EdaPage (objects cannot be shared across pages).
 */

#define EDA_OBJECT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EDA_TYPE_OBJECT, EdaObjectPrivate))

G_DEFINE_ABSTRACT_TYPE (EdaObject, eda_object, G_TYPE_INITIALLY_UNOWNED);

struct _EdaObjectPrivate
{
    /* The page this object belongs to */
    EdaPage *page;

    EdaConfig *cfg;
};

enum
{
    PRE_CHANGED,
    CHANGED,
    MIRROR,
    ROTATE,
    TRANSLATE,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
eda_object_dispose(GObject *obj_)
{
    EdaObject *obj = EDA_OBJECT(obj_);

    obj->priv->page = NULL;

    G_OBJECT_CLASS(eda_object_parent_class)->dispose(obj_);
}

static EdaObject *
eda_object_real_copy(EdaObject *obj)
{
    EdaObject *copy;

    copy = g_object_new(G_TYPE_FROM_INSTANCE(obj), NULL);

    if(obj->priv->page)
        eda_page_add_object(obj->priv->page, copy);

    copy->priv->cfg = obj->priv->cfg;
    return copy;
}

static void
eda_object_class_init(EdaObjectClass *klass)
{
    GObjectClass *o_klass = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(EdaObjectPrivate));

    o_klass->dispose = eda_object_dispose;

    klass->copy = eda_object_real_copy;

    klass->mirror = NULL;
    klass->rotate = NULL;
    klass->translate = NULL;

    /**
     * EdaObject::pre-changed:
     * @obj: the object that received the signal
     *
     * The ::pre-changed signal is emitted before an object is changed.
     */
    signals[PRE_CHANGED] =
        g_signal_new("pre-changed",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * EdaObject::changed:
     * @obj: the object that received the signal
     *
     * The ::changed signal is emitted after an object is changed.
     */
    signals[CHANGED] =
        g_signal_new("changed",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * EdaObject::mirror:
     * @obj: the object that received the signal
     * @x: the x coordinate of the mirror point
     * @y: the y coordinate of the mirror point
     *
     * The ::mirror signal is emitted each time an object is mirrored via
     * a call to eda_object_mirror().
     */
    signals[MIRROR] =
        g_signal_new("mirror",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     _eda_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);

    /**
     * EdaObject::rotate:
     * @obj: an #EdaObject
     * @x: the x coordinate of the rotation center
     * @y: the y coordinate of the rotation center
     * @angle: the angle of rotation in degrees
     *
     * The ::rotate signal is emitted each time an object is rotated via
     * a call to eda_object_rotate().
     */
    signals[ROTATE] =
        g_signal_new("rotate",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     _eda_marshal_VOID__INT_INT_INT,
                     G_TYPE_NONE,
                     3, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

    /**
     * EdaObject::translate:
     * @obj: an #EdaObject
     * @dx: change in the horizontal direction
     * @dy: change in the vertical direction
     *
     * The ::translate signal is emitted each time an object is translated via
     * a call to eda_object_translate().
     */
    signals[TRANSLATE] =
        g_signal_new("translate",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     _eda_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);
}

static void
eda_object_init(EdaObject *obj)
{
    EdaObjectPrivate *priv;

    obj->priv = priv = EDA_OBJECT_GET_PRIVATE(obj);

    priv->page = NULL;
    priv->cfg = eda_config_get_default();
}

/**
 * eda_object_emit_pre_changed:
 * @obj: an #EdaObject
 *
 * Emits the #EdaObject::pre-changed signal.
 */
void
eda_object_emit_pre_changed(EdaObject *obj)
{
    g_return_if_fail(EDA_IS_OBJECT(obj));

    g_signal_emit(obj, signals[PRE_CHANGED], 0);
}

/**
 * eda_object_emit_changed:
 * @obj: an #EdaObject
 *
 * Emits the #EdaObject::changed signal.
 */
void
eda_object_emit_changed(EdaObject *obj)
{
    g_return_if_fail(EDA_IS_OBJECT(obj));

    g_signal_emit(obj, signals[CHANGED], 0);
}

/**
 * eda_object_copy:
 * @obj: an #EdaObject
 *
 * Copies an object.
 *
 * Returns: an #EdaObject that is a copy of @obj
 */
EdaObject *
eda_object_copy(EdaObject *obj)
{
    EdaObjectClass *klass;

    g_return_val_if_fail(EDA_IS_OBJECT(obj), NULL);

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->copy)
        return klass->copy(obj);
    return NULL;
}

/**
 * eda_object_get_bounds:
 * @obj: an #EdaObject
 * @left: location to store the leftmost coord
 * @top: location to store the topmost coord
 * @right: location to store the rightmost coord
 * @bottom: location to store the bottommost coord
 *
 * Returns the object's bounds in the form of a rectangle determined by
 * (@left, @top) and (@right, @bottom).
 *
 * Returns: the object's bounds
 */
void
eda_object_get_bounds(EdaObject *obj, gint *left, gint *top,
                      gint *right, gint *bottom)
{
    EdaObjectClass *klass;

    g_return_if_fail(EDA_IS_OBJECT(obj));

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->get_bounds)
        klass->get_bounds(obj, left, top, right, bottom);
}

/**
 * eda_object_get_page:
 * @obj: an #EdaObject
 *
 * Returns the object's page, or %NULL if the object is not
 * part of a page.
 *
 * Returns: an #EdaPage, or %NULL
 */
EdaPage *
eda_object_get_page(EdaObject *obj)
{
    g_return_val_if_fail(EDA_IS_OBJECT(obj), NULL);

    return obj->priv->page;
}

void
_eda_object_set_page(EdaObject *obj, EdaPage *page)
{
    g_return_if_fail(EDA_IS_OBJECT(obj));

    obj->priv->page = page;
}

/**
 * eda_object_to_string:
 * @obj: an #EdaObject
 *
 * Returns a newly allocated string that represents @obj.
 * The caller is responsible for freeing the string.
 *
 * Returns: a string representing the object
 */
gchar *
eda_object_to_string(EdaObject *obj)
{
    EdaObjectClass *klass;

    g_return_val_if_fail(EDA_IS_OBJECT(obj), NULL);

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->to_string)
        return klass->to_string(obj);
    return NULL;
}

/**
 * eda_object_from_string:
 * @buf: the buffer to read from
 * @size: size of the buffer in bytes
 * @offset: number of bytes from the beginning of @buf at
 *          which the object data resides
 * @err: the error return location, or %NULL
 *
 * Creates a new object of an appropriate type from a string. The @offset
 * argument specifies the position in @buf at which the object data begins. If
 * the function successfully reads a single object from the buffer, the @offset
 * is updated to point just after the read data.
 *
 * If an error occurs, the function returns %NULL and sets the error location
 * to the appropriate #GError (if @err is not %NULL). In this case the @offset
 * is left unchanged.
 *
 * Returns: an #EdaObject, or %NULL if an error occurred
 */
EdaObject *
eda_object_from_string(const gchar *buf, size_t size, size_t *offset,
                       GError **err)
{
    EdaObjectClass *klass;

    g_return_val_if_fail(err == NULL || *err == NULL, NULL);

    klass = _eda_object_class_from_char(buf[*offset]);
    if (!klass || !klass->from_string) {
        g_set_error(err, EDA_ERROR, EDA_ERROR_DESERIALIZATION,
                    _("invalid object at offset %zu"), *offset);
        return NULL;
    }

    return klass->from_string(buf, size, offset, err);
}

/**
 * eda_object_mirror:
 * @obj: an #EdaObject
 * @x: the x coordinate of the mirror point
 * @y: the y coordinate of the mirror point
 *
 * Mirrors the object horizontally at the point (@x, @y).
 * Emits the #EdaObject::mirror signal.
 */
void
eda_object_mirror(EdaObject *obj, gint x, gint y)
{
    EdaObjectClass *klass;

    g_return_if_fail(EDA_IS_OBJECT(obj));

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->mirror) {
        eda_object_emit_pre_changed(obj);

        klass->mirror(obj, x, y);
        g_signal_emit(obj, signals[MIRROR], 0, x, y);

        eda_object_emit_changed(obj);
    }
}

/**
 * eda_object_rotate:
 * @obj: an #EdaObject
 * @x: the x coordinate of the rotation center
 * @y: the y coordinate of the rotation center
 * @angle: the angle of rotation in degrees (must be a multiple of 90)
 *
 * Rotates the object around (@x, @y) by @angle.
 * Emits the #EdaObject::rotate signal.
 */
void
eda_object_rotate(EdaObject *obj, gint x, gint y, gint angle)
{
    EdaObjectClass *klass;

    g_return_if_fail(EDA_IS_OBJECT(obj));

    if(angle == 0)
        return;

    g_return_if_fail(angle % 90 == 0);

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->rotate) {
        eda_object_emit_pre_changed(obj);

        klass->rotate(obj, x, y, angle);
        g_signal_emit(obj, signals[ROTATE], 0, x, y, angle);

        eda_object_emit_changed(obj);
    }
}

/**
 * eda_object_translate:
 * @obj: an #EdaObject
 * @dx: change in the horizontal direction
 * @dy: change in the vertical direction
 *
 * Translates the object by (@dx, @dy).
 * Emits the #EdaObject::translate signal.
 */
void
eda_object_translate(EdaObject *obj, gint dx, gint dy)
{
    EdaObjectClass *klass;

    g_return_if_fail(EDA_IS_OBJECT(obj));

    klass = EDA_OBJECT_GET_CLASS(obj);
    if (klass->translate) {
        eda_object_emit_pre_changed(obj);

        klass->translate(obj, dx, dy);
        g_signal_emit(obj, signals[TRANSLATE], 0, dx, dy);

        eda_object_emit_changed(obj);
    }
}

EdaConfig *
eda_object_get_config(EdaObject *obj)
{
    return obj->priv->cfg;
}

void
eda_object_set_config(EdaObject *obj, EdaConfig *cfg)
{
    obj->priv->cfg = cfg;
}

