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

#ifndef EDA_NET_H
#define EDA_NET_H

#include <glib-object.h>

#include "edaobject.h"

G_BEGIN_DECLS

#define EDA_TYPE_NET                  (eda_net_get_type ())
#define EDA_NET(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDA_TYPE_NET, EdaNet))
#define EDA_IS_NET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDA_TYPE_NET))
#define EDA_NET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), EDA_TYPE_NET, EdaNetClass))
#define EDA_IS_NET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), EDA_TYPE_NET))
#define EDA_NET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), EDA_TYPE_NET, EdaNetClass))

typedef struct _EdaNet        EdaNet;
typedef struct _EdaNetClass   EdaNetClass;
typedef struct _EdaNetPrivate EdaNetPrivate;

struct _EdaNet
{
    EdaObject parent_instance;

    EdaNetPrivate *priv;
};

struct _EdaNetClass
{
    EdaObjectClass parent_class;
};

GType eda_net_get_type (void);

EdaObject *eda_net_new(gint color, gint x1, gint y1, gint x2, gint y2);

gint eda_net_get_color(EdaNet *net);
void eda_net_set_color(EdaNet *net, gint color);

void eda_net_get_coords(EdaNet *net, gint *x1, gint *y1, gint *x2, gint *y2);
void eda_net_set_coords(EdaNet *net, gint x1, gint y1, gint x2, gint y2);

G_END_DECLS

#endif

