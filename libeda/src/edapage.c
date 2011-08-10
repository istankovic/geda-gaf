#include <stdio.h>

#include <libeda/edapage.h>
#include <libeda/edaobject.h>
#include <libeda/edaerrors.h>

#include "version.h"
#include "i18n.h"

/* XXX */
#define FILEFORMAT_VERSION 2

/**
 * SECTION:edapage
 * @short_description: An object representing a schematic page.
 * @title: EdaPage
 *
 * A page serves as a container for various objects (#EdaObject instances).
 * Each #EdaPage object represents a single schematic page that may have been
 * loaded from a file, loaded from a string or created programatically.
 * One can also serialize pages to strings and files.
 */

#define EDA_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EDA_TYPE_PAGE, EdaPagePrivate))

G_DEFINE_TYPE (EdaPage, eda_page, G_TYPE_OBJECT);

void _eda_object_set_page(EdaObject *obj, EdaPage *page);

struct _EdaPagePrivate
{
    /* A list of EdaObject instances */
    GList *objects;

    /* The file this page was loaded from */
    gchar *filename;
};

enum
{
    ADD_OBJECT,
    REMOVE_OBJECT,
    PRE_OBJECT_CHANGED,
    OBJECT_CHANGED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
eda_page_dispose(GObject *obj)
{
    EdaPage *page = EDA_PAGE(obj);

    eda_page_remove_all_objects(page);

    G_OBJECT_CLASS(eda_page_parent_class)->dispose(obj);
}

static void
eda_page_finalize(GObject *obj)
{
    EdaPage *page = EDA_PAGE(obj);

    g_free(page->priv->filename);
    page->priv->filename = NULL;

    G_OBJECT_CLASS(eda_page_parent_class)->finalize(obj);
}

static void
object_pre_changed(EdaObject *obj, gpointer data)
{
    EdaPage *page = EDA_PAGE(data);

    g_signal_emit(page, signals[PRE_OBJECT_CHANGED], 0, obj);
}

static void
object_changed(EdaObject *obj, gpointer data)
{
    EdaPage *page = EDA_PAGE(data);

    g_signal_emit(page, signals[OBJECT_CHANGED], 0, obj);
}

static void
eda_page_real_add_object(EdaPage *page, EdaObject *obj)
{
    g_object_ref_sink(obj);
    page->priv->objects = g_list_append(page->priv->objects, obj);
    _eda_object_set_page(obj, page);
    g_signal_connect(obj, "pre-changed", G_CALLBACK(object_pre_changed), page);
    g_signal_connect(obj, "changed", G_CALLBACK(object_changed), page);
}

static void
eda_page_real_remove_object(EdaPage *page, EdaObject *obj)
{
    _eda_object_set_page(obj, NULL);
    page->priv->objects = g_list_remove(page->priv->objects, obj);
    g_signal_handlers_disconnect_by_func(obj, object_pre_changed, page);
    g_signal_handlers_disconnect_by_func(obj, object_changed, page);
    g_object_unref(obj);
}

static void
eda_page_class_init(EdaPageClass *klass)
{
    GObjectClass *o_klass = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(EdaPagePrivate));

    o_klass->dispose = eda_page_dispose;
    o_klass->finalize = eda_page_finalize;

    klass->add_object = eda_page_real_add_object;
    klass->remove_object = eda_page_real_remove_object;

    /**
     * EdaPage::add-object:
     * @obj: the object that is being added to the page
     *
     * The ::add-object signal is emitted when an object
     * is added to the page.
     */
    signals[ADD_OBJECT] =
        g_signal_new("add-object",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(EdaPageClass, add_object),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE,
                     1, EDA_TYPE_OBJECT);

    /**
     * EdaPage::remove-object:
     * @obj: the object that is being removed from the page
     *
     * The ::remove-object signal is emitted when an object
     * is removed from the page.
     */
    signals[REMOVE_OBJECT] =
        g_signal_new("remove-object",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(EdaPageClass, remove_object),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE,
                     1, EDA_TYPE_OBJECT);

    /**
     * EdaPage::pre-object-changed:
     * @obj: the object that is going to be changed
     *
     * The ::pre-object-changed signal is emitted just before an object
     * in the page is changed.
     */
    signals[PRE_OBJECT_CHANGED] =
        g_signal_new("pre-object-changed",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE,
                     1, EDA_TYPE_OBJECT);

    /**
     * EdaPage::object-changed:
     * @obj: the object that was changed
     *
     * The ::object-changed signal is emitted just after an object
     * in the page is changed.
     */
    signals[OBJECT_CHANGED] =
        g_signal_new("object-changed",
                     G_TYPE_FROM_CLASS(o_klass),
                     G_SIGNAL_RUN_FIRST,
                     0, NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE,
                     1, EDA_TYPE_OBJECT);
}

static void
eda_page_init(EdaPage *page)
{
    EdaPagePrivate *priv;

    page->priv = priv = EDA_PAGE_GET_PRIVATE(page);

    priv->objects = NULL;
    priv->filename = NULL;
}

/**
 * eda_page_new:
 *
 * Creates a new, empty #EdaPage.
 *
 * Returns: an #EdaPage.
 */
EdaPage *
eda_page_new()
{
    return g_object_new(EDA_TYPE_PAGE, NULL);
}

/**
 * eda_page_get_filename:
 * @page: an #EdaPage
 *
 * Returns the name of the file that the page was loaded from.
 * The returned string is owned by the page and should not be
 * freed.
 *
 * Returns: the string containing the file name, or %NULL if
 *          the page was not loaded from a file
 */
const gchar *
eda_page_get_filename(EdaPage *page)
{
    return page->priv->filename;
}

/**
 * eda_page_add_object:
 * @page: an #EdaPage
 * @obj: an #EdaObject
 *
 * Adds @obj to the @page. The page takes ownership of the
 * object (internally, %g_object_ref_sink is called on @obj).
 * Emits the #EdaPage::add-object signal.
 */
void
eda_page_add_object(EdaPage *page, EdaObject *obj)
{
    g_return_if_fail(EDA_IS_PAGE(page));
    g_return_if_fail(EDA_IS_OBJECT(obj));
    g_return_if_fail(eda_object_get_page(obj) == NULL);

    g_signal_emit(page, signals[ADD_OBJECT], 0, obj);
}

/**
 * eda_page_remove_object:
 * @page: an #EdaPage
 * @obj: an #EdaObject
 *
 * Removes @obj from the @page and decreases the reference count of @obj.
 * The object's page must be @page. If you want to keep the object alive,
 * increase its reference count prior to calling this function.
 * Emits the #EdaPage::remove-object signal.
 */
void
eda_page_remove_object(EdaPage *page, EdaObject *obj)
{
    g_return_if_fail(EDA_IS_PAGE(page));
    g_return_if_fail(EDA_IS_OBJECT(obj));
    g_return_if_fail(g_list_find(page->priv->objects, obj) != NULL);

    g_signal_emit(page, signals[REMOVE_OBJECT], 0, obj);
}

/**
 * eda_page_remove_all_objects:
 * @page: an #EdaPage
 *
 * Removes all objects from the @page.
 *
 * See also eda_page_remove_object().
 */
void
eda_page_remove_all_objects(EdaPage *page)
{
    GList *p;
    EdaObject *obj;

    g_return_if_fail(EDA_IS_PAGE(page));

    for (p = page->priv->objects; p != NULL; ) {
        obj = EDA_OBJECT(p->data);
        _eda_object_set_page(obj, NULL);
        p = g_list_delete_link(p, p);
        g_object_unref(obj);
    }
    page->priv->objects = NULL;
}

/**
 * eda_page_get_objects:
 * @page: an #EdaPage
 *
 * Returns a newly-allocated list of objects in the page.
 * The caller is responsible for freeing the list.
 *
 * Returns: the list of objects, or %NULL if there are none.
 */
GList *
eda_page_get_objects(EdaPage *page)
{
    g_return_val_if_fail(EDA_IS_PAGE(page), NULL);

    return g_list_copy(page->priv->objects);
}

/**
 * eda_page_new_from_string:
 * @buf: the buffer to read from
 * @size: size of the buffer in bytes
 * @err: the error return location, or %NULL
 *
 * Creates a new page from a string. If the string contained in @buf is not a
 * valid #EdaPage representation, the function will return %NULL and set the
 * error location to the appropriate #GError (if @err is not %NULL).
 *
 * Returns: an #EdaPage, or %NULL if an error occurred
 */
EdaPage *
eda_page_new_from_string(const gchar *buf, size_t size, GError **err)
{
    EdaPage *page;
    EdaObject *obj;
    int consumed;
    size_t offset = 0;
    unsigned int release_ver;
    unsigned int fileformat_ver;

    g_return_val_if_fail(buf != NULL, NULL);
    g_return_val_if_fail(err == NULL || *err == NULL, NULL);

    if (sscanf(buf, "v %u %u\n%n", &release_ver, &fileformat_ver, &consumed) != 2) {
        g_set_error(err, EDA_ERROR, EDA_ERROR_DESERIALIZATION,
                    _("invalid header at offset %zu"), offset);
        return NULL;
    }

    offset += consumed;

    page = eda_page_new();
    while (offset < size) {
        obj = eda_object_from_string(buf, size, &offset, err);
        if (obj == NULL) {
            g_object_unref(page);
            return NULL;
        }

        eda_page_add_object(page, obj);
    }

    return page;
}

/**
 * eda_page_new_from_file:
 * @filename: the file to read from
 * @err: the error return location, or %NULL
 *
 * Loads a page from a file. If the file does not contain a valid #EdaPage
 * representation, the function will return %NULL and set the error location to
 * the appropriate #GError (if @err is not %NULL).
 *
 * Returns: an #EdaPage, or %NULL if an error occurred
 */
EdaPage *
eda_page_new_from_file(const gchar *filename, GError **err)
{
    char *buf;
    size_t size;
    EdaPage *page;

    g_return_val_if_fail(filename != NULL, NULL);
    g_return_val_if_fail(err == NULL || *err == NULL, NULL);

    if (!g_file_get_contents(filename, &buf, &size, err))
        return NULL;

    page = eda_page_new_from_string(buf, size, err);
    if (page)
        page->priv->filename = g_strdup(filename);
    else
        g_prefix_error(err, _("error loading page from %s: "), filename);

    g_free(buf);
    return page;
}

static inline const gchar *
file_format_header()
{
    return "v " PACKAGE_DATE_VERSION " " G_STRINGIFY(FILEFORMAT_VERSION) "\n";
}

/**
 * eda_page_to_string:
 * @page: an #EdaPage
 *
 * Returns a newly allocated string that represents @page.
 * The caller is responsible for freeing the string.
 *
 * Returns: a string representing the page
 */
gchar *
eda_page_to_string(EdaPage *page)
{
    GList *p;
    GString *s;
    gchar *tmp;

    s = g_string_new(file_format_header());

    for (p = page->priv->objects; p != NULL; p = g_list_next(p)) {
        tmp = eda_object_to_string(EDA_OBJECT(p->data));
        if (tmp) {
            g_string_append(s, tmp);
            g_free(tmp);
        }
    }

    return g_string_free(s, FALSE);
}

