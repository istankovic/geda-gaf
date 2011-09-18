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

#ifndef EDA_PIN_H
#define EDA_PIN_H

#include <glib-object.h>

#include "edaobject.h"
#include "edaenums.h"

G_BEGIN_DECLS

#define EDA_TYPE_PIN                  (eda_pin_get_type ())
#define EDA_PIN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDA_TYPE_PIN, EdaPin))
#define EDA_IS_PIN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDA_TYPE_PIN))
#define EDA_PIN_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), EDA_TYPE_PIN, EdaPinClass))
#define EDA_IS_PIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), EDA_TYPE_PIN))
#define EDA_PIN_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), EDA_TYPE_PIN, EdaPinClass))

typedef struct _EdaPin        EdaPin;
typedef struct _EdaPinClass   EdaPinClass;
typedef struct _EdaPinPrivate EdaPinPrivate;

struct _EdaPin
{
    EdaObject parent_instance;

    EdaPinPrivate *priv;
};

struct _EdaPinClass
{
    EdaObjectClass parent_class;
};

GType eda_pin_get_type (void);

EdaObject *eda_pin_new(EdaPinKind kind, gint whichend, gint x1, gint y1,
                       gint x2, gint y2, gint color);

gint eda_pin_get_color(EdaPin *pin);
void eda_pin_set_color(EdaPin *pin, gint color);

void eda_pin_get_coords(EdaPin *pin, gint *x1, gint *y1, gint *x2, gint *y2);
void eda_pin_set_coords(EdaPin *pin, gint x1, gint y1, gint x2, gint y2);

void eda_pin_get_position(EdaPin *pin, gint *x, gint *y);

G_END_DECLS

#endif

