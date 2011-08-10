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

#ifndef SIGNALTRACKER_H
#define SIGNALTRACKER_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _SignalTracker SignalTracker;

SignalTracker *signal_tracker_new();

void signal_tracker_track(SignalTracker *st,
                          gpointer instance, const gchar *name);

gboolean signal_tracker_match(SignalTracker *st,
                              gpointer instance, const gchar *name);

gboolean signal_tracker_is_empty(SignalTracker *st);

void signal_tracker_free(SignalTracker *st);

G_END_DECLS

#endif

