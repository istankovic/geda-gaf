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

#ifndef EDA_OBJECT_H
#define EDA_OBJECT_H

#include <glib-object.h>

#include <libeda/edapage.h>
#include <libeda/edaconfig.h>

G_BEGIN_DECLS

#define EDA_TYPE_OBJECT                  (eda_object_get_type ())
#define EDA_OBJECT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDA_TYPE_OBJECT, EdaObject))
#define EDA_IS_OBJECT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDA_TYPE_OBJECT))
#define EDA_OBJECT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), EDA_TYPE_OBJECT, EdaObjectClass))
#define EDA_IS_OBJECT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), EDA_TYPE_OBJECT))
#define EDA_OBJECT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), EDA_TYPE_OBJECT, EdaObjectClass))

typedef struct _EdaObject        EdaObject;
typedef struct _EdaObjectClass   EdaObjectClass;
typedef struct _EdaObjectPrivate EdaObjectPrivate;

struct _EdaObject
{
    GInitiallyUnowned parent_instance;

    EdaObjectPrivate *priv;
};

struct _EdaObjectClass
{
    GInitiallyUnownedClass parent_class;

    EdaObject *(*copy)(EdaObject *obj);

    gchar *(*to_string)(EdaObject *obj);
    EdaObject *(*from_string)(const gchar *buf, size_t size,
                              size_t *offset, GError **err);

    void (*get_bounds)(EdaObject *obj, gint *left, gint *top,
                       gint *right, gint *bottom);

    void (*mirror)(EdaObject *obj, gint x, gint y);
    void (*translate)(EdaObject *obj, gint dx, gint dy);
    void (*rotate)(EdaObject *obj, gint x, gint y, gint angle);
};

GType eda_object_get_type (void);

void eda_object_emit_pre_changed(EdaObject *obj);
void eda_object_emit_changed(EdaObject *obj);

EdaPage *eda_object_get_page(EdaObject *obj);

EdaConfig *eda_object_get_config(EdaObject *obj);
void eda_object_set_config(EdaObject *obj, EdaConfig *cfg);

EdaObject *eda_object_copy(EdaObject *obj);

gchar *eda_object_to_string(EdaObject *obj);
EdaObject *eda_object_from_string(const gchar *buf, size_t size,
                                  size_t *offset, GError **err);

void eda_object_get_bounds(EdaObject *obj, gint *left, gint *top,
                           gint *right, gint *bottom);

void eda_object_mirror(EdaObject *obj, gint x, gint y);
void eda_object_translate(EdaObject *obj, gint dx, gint dy);
void eda_object_rotate(EdaObject *obj, gint x, gint y, gint angle);

G_END_DECLS

#endif

