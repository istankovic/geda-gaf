/* gEDA - GPL Electronic Design Automation
 * libeda - gEDA's Library
 * Copyright (C) 2011      gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

#ifndef EDA_ERRORS_H
#define EDA_ERRORS_H

G_BEGIN_DECLS

/*! Domain for GErrors originating in libeda. */
#define EDA_ERROR eda_error_quark ()

/**
 * EdaError:
 * @EDA_ERROR_SCHEME: A Scheme error occurred.
 * @EDA_ERROR_RC_TWICE: Attempted to read a configuration file twice.
 * @EDA_ERROR_DESERIALIZATION: A deserialization error occurred.
 *
 * Used to indicate the type of errors in libeda.
 */
typedef enum {
  EDA_ERROR_SCHEME,
  EDA_ERROR_RC_TWICE,
  EDA_ERROR_DESERIALIZATION
} EdaError;

GQuark eda_error_quark (void);

G_END_DECLS

#endif

