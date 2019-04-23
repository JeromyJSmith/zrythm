/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __GUI_WIDGETS_FIRST_RUN_ASSISTANT_H__
#define __GUI_WIDGETS_FIRST_RUN_ASSISTANT_H__

#include "audio/channel.h"
#include "gui/widgets/meter.h"

#include <gtk/gtk.h>

#define FIRST_RUN_ASSISTANT_WIDGET_TYPE \
  (first_run_assistant_widget_get_type ())
G_DECLARE_FINAL_TYPE (FirstRunAssistantWidget,
                      first_run_assistant_widget,
                      Z,
                      FIRST_RUN_ASSISTANT_WIDGET,
                      GtkAssistant)

typedef struct _MidiControllerMbWidget
  MidiControllerMbWidget;

typedef struct _FirstRunAssistantWidget
{
  GtkAssistant        parent_instance;
  GtkComboBox *       language_cb;
  GtkLabel *          locale_not_available;
  GtkFileChooserButton * fc_btn;
  GtkButton *         reset;
  MidiControllerMbWidget * midi_devices;
  GtkButton *          test_backends;
  GtkComboBox *        audio_backend;
  GtkComboBox *        midi_backend;
} FirstRunAssistantWidget;

/**
 * Creates a FirstRunAssistant.
 */
FirstRunAssistantWidget *
first_run_assistant_widget_new (
  GtkWindow * parent);

#endif
