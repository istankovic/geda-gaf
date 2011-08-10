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

#ifndef EDA_CONFIG_H
#define EDA_CONFIG_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _EdaConfig EdaConfig;

EdaConfig *eda_config_get_default();

/* getters */
gint eda_config_lookup_int(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err);
gdouble eda_config_lookup_double(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err);
gboolean eda_config_lookup_boolean(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err);

gint eda_config_get_int(EdaConfig *cfg, const gchar *group, const gchar *key);
gdouble eda_config_get_double(EdaConfig *cfg, const gchar *group, const gchar *key);
gboolean eda_config_get_boolean(EdaConfig *cfg, const gchar *group, const gchar *key);

/* setters */
void eda_config_set_int(EdaConfig *cfg, const char *group, const char *key, gint value);
void eda_config_set_double(EdaConfig *cfg, const char *group, const char *key, gdouble value);
void eda_config_set_boolean(EdaConfig *cfg, const char *group, const char *key, gboolean value);

G_END_DECLS

#endif


