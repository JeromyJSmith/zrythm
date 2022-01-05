/*
 * Copyright (C) 2022 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Pango utils.
 */

#ifndef __UTILS_PANGO_H__
#define __UTILS_PANGO_H__

#include <gtk/gtk.h>

/**
 * @addtogroup utils
 *
 * @{
 */

PangoLayout *
z_pango_create_layout_from_description (
  GtkWidget *            widget,
  PangoFontDescription * descr);

/**
 * @}
 */

#endif /* __UTILS_PANGO_H__ */
