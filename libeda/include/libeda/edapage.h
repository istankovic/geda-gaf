/* gEDA - GPL Electronic Design Automation
 * libeda - gEDA's Library
 * Copyright (C) 2011 gEDA developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef EDA_PAGE_H
#define EDA_PAGE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define EDA_TYPE_PAGE                  (eda_page_get_type ())
#define EDA_PAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDA_TYPE_PAGE, EdaPage))
#define EDA_IS_PAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDA_TYPE_PAGE))
#define EDA_PAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), EDA_TYPE_PAGE, EdaPageClass))
#define EDA_IS_PAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), EDA_TYPE_PAGE))
#define EDA_PAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), EDA_TYPE_PAGE, EdaPageClass))

typedef struct _EdaObject EdaObject;

typedef struct _EdaPage        EdaPage;
typedef struct _EdaPageClass   EdaPageClass;
typedef struct _EdaPagePrivate EdaPagePrivate;

struct _EdaPage
{
    GObject parent_instance;

    EdaPagePrivate *priv;
};

struct _EdaPageClass
{
    GObjectClass parent_class;

    /* signals */
    void (*add_object)(EdaPage *page, EdaObject *obj);
    void (*remove_object)(EdaPage *page, EdaObject *obj);
};

GType eda_page_get_type (void);

EdaPage *eda_page_new();
EdaPage *eda_page_new_from_string(const gchar *buf, size_t size, GError **err);
EdaPage *eda_page_new_from_file(const gchar *filename, GError **err);

const gchar *eda_page_get_filename(EdaPage *page);

gchar *eda_page_to_string(EdaPage *page);

GList *eda_page_get_objects(EdaPage *page);
void eda_page_add_object(EdaPage *page, EdaObject *obj);
void eda_page_remove_object(EdaPage *page, EdaObject *obj);
void eda_page_remove_all_objects(EdaPage *page);

G_END_DECLS

#endif

