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

/**
 * \file
 *
 */

#include "audio/engine.h"
#include "audio/engine_jack.h"
#include "audio/engine_pa.h"
#include "gui/widgets/first_run_assistant.h"
#include "gui/widgets/midi_controller_mb.h"
#include "utils/arrays.h"
#include "utils/flags.h"
#include "utils/gtk.h"
#include "utils/localization.h"
#include "utils/io.h"
#include "utils/resources.h"
#include "utils/ui.h"
#include "zrythm.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE (FirstRunAssistantWidget,
               first_run_assistant_widget,
               GTK_TYPE_ASSISTANT)

static void
on_reset_clicked (
  GtkButton *widget,
  FirstRunAssistantWidget * self)
{
  g_message ("reset");
  char * dir =
    g_build_filename (io_get_home_dir (),
                      "zrythm",
                      NULL);
  gtk_file_chooser_select_filename (
    GTK_FILE_CHOOSER (self->fc_btn),
    dir);
  g_settings_set_string (
    S_GENERAL, "dir", dir);
  g_free (dir);
}

/**
 * Validates the current audio/MIDI backend selection.
 */
static int
audio_midi_backend_selection_validate (
  FirstRunAssistantWidget * self)
{
  AudioBackend ab =
    gtk_combo_box_get_active (self->audio_backend);
  MidiBackend mb =
    gtk_combo_box_get_active (self->midi_backend);

  char * msg;

  /* test audio backends */
  if (ab == AUDIO_BACKEND_JACK)
    {
#ifdef HAVE_JACK
      if (engine_jack_test (GTK_WINDOW (self)))
        return 1;
#else
      msg =
        _("JACK functionality is disabled");
      ui_show_error_message (
        self, msg);
      return 1;
#endif
    }
  else if (ab == AUDIO_BACKEND_ALSA)
    {
      ui_show_error_message (
        self, "ALSA backend not implemented");
      return 1;
    }
  else if (ab == AUDIO_BACKEND_PORT_AUDIO)
    {
#ifdef HAVE_PORT_AUDIO
      if (engine_pa_test (GTK_WINDOW (self)))
        return 1;
#else
      msg =
        _("PortAudio functionality is disabled");
      ui_show_error_message (
        self, msg);
      return 1;
#endif
    }

  /* test MIDI backends */
  if (mb == MIDI_BACKEND_JACK)
    {
#ifdef HAVE_JACK
      if (engine_jack_test (GTK_WINDOW (self)))
        return 1;
#else
      msg =
        _("JACK functionality is disabled");
      ui_show_error_message (
        self, msg);
      return 1;
#endif
    }

  return 0;
}

static void
on_test_backends_clicked (
  GtkButton * widget,
  FirstRunAssistantWidget * self)
{
  audio_midi_backend_selection_validate (self);
}

static void
on_audio_backend_changed (
  GtkComboBox *widget,
  FirstRunAssistantWidget * self)
{
  AudioBackend ab =
    gtk_combo_box_get_active (widget);

  /* update settings */
  g_settings_set_enum (
    S_PREFERENCES,
    "audio-backend",
    ab);
}

static void
on_midi_backend_changed (
  GtkComboBox *widget,
  FirstRunAssistantWidget * self)
{
  MidiBackend mb =
    gtk_combo_box_get_active (widget);

  /* update settings */
  g_settings_set_enum (
    S_PREFERENCES,
    "midi-backend",
    mb);
}

static void
on_language_changed (
  GtkComboBox *widget,
  FirstRunAssistantWidget * self)
{
  UiLanguage lang =
    gtk_combo_box_get_active (widget);

  /* update settings */
  g_settings_set_enum (
    S_PREFERENCES,
    "language",
    lang);

  /* if locale exists */
  if (localization_init ())
    {
      /* enable "Next" */
      gtk_assistant_set_page_complete (
        GTK_ASSISTANT (self),
        gtk_assistant_get_nth_page (
          GTK_ASSISTANT (self), 0),
        1);
      gtk_widget_set_visible (
        GTK_WIDGET (self->locale_not_available),
        F_NOT_VISIBLE);
    }
  /* locale doesn't exist */
  else
    {
      /* disable "Next" */
      gtk_assistant_set_page_complete (
        GTK_ASSISTANT (self),
        gtk_assistant_get_nth_page (
          GTK_ASSISTANT (self), 0),
        0);

      /* show warning */
      char * template =
        _("A locale for the language you have selected is \
not available. Please enable one first using \
the steps below and try again.\n\
1. Uncomment any locale starting with the \
language code <b>%s</b> in <b>/etc/locale.gen</b> (needs \
root privileges)\n\
2. Run <b>locale-gen</b> as root\n\
3. Restart Zrythm");
      char code[3];
      localization_get_string_code (lang, code);
      char * str =
        g_strdup_printf (
          template,
          code);
      gtk_label_set_markup (
        self->locale_not_available,
        str);
      g_free (str);
      gtk_widget_set_visible (
        GTK_WIDGET (self->locale_not_available),
        F_VISIBLE);
    }
}

static void
on_file_set (
  GtkFileChooserButton *widget,
  FirstRunAssistantWidget * self)
{
  GFile * file =
    gtk_file_chooser_get_file (
      GTK_FILE_CHOOSER (widget));
  char * str =
    g_file_get_path (file);
  g_settings_set_string (
    S_GENERAL, "dir", str);
  g_free (str);
}

FirstRunAssistantWidget *
first_run_assistant_widget_new (
  GtkWindow * parent)
{
  FirstRunAssistantWidget * self =
    g_object_new (
      FIRST_RUN_ASSISTANT_WIDGET_TYPE,
      "modal", 1,
      "urgency-hint", 1,
      NULL);

  /* setup languages */
  ui_setup_language_combo_box (
    self->language_cb);

  /* setup backends */
  ui_setup_audio_backends_combo_box (
    self->audio_backend);
  ui_setup_midi_backends_combo_box (
    self->midi_backend);

  /* set zrythm dir */
  char * dir;
  dir = zrythm_get_dir (ZRYTHM);
  if (strlen (dir) == 0)
    {
      g_free (dir);
      dir = g_build_filename (io_get_home_dir (),
                        "zrythm",
                        NULL);
    }
  io_mkdir (dir);
  gtk_file_chooser_set_current_folder (
    GTK_FILE_CHOOSER (self->fc_btn),
    dir);
  g_settings_set_string (
    S_GENERAL, "dir", dir);
  g_free (dir);

  /* set midi devices */
  midi_controller_mb_widget_setup (
    self->midi_devices);

  g_message ("n pages %d",
             gtk_assistant_get_n_pages (GTK_ASSISTANT (self)));

  gtk_window_set_position (
    GTK_WINDOW (self), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_transient_for (
    GTK_WINDOW (self), parent);
  gtk_window_set_attached_to (
    GTK_WINDOW (self), GTK_WIDGET (parent));
  gtk_window_set_keep_above (
    GTK_WINDOW (self), 1);

  return self;
}

static void
first_run_assistant_widget_class_init (
  FirstRunAssistantWidgetClass * _klass)
{
  GtkWidgetClass * klass =
    GTK_WIDGET_CLASS (_klass);
  resources_set_class_template (
    klass, "first_run_assistant.ui");

  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    language_cb);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    audio_backend);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    midi_backend);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    locale_not_available);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    fc_btn);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    reset);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    test_backends);
  gtk_widget_class_bind_template_child (
    klass,
    FirstRunAssistantWidget,
    midi_devices);
}

static void
first_run_assistant_widget_init (
  FirstRunAssistantWidget * self)
{
  /* needed on windows at least so the template
   * engine knows what the widget is */
  MidiControllerMbWidget * w =
    g_object_new (
      MIDI_CONTROLLER_MB_WIDGET_TYPE, NULL);
  gtk_widget_destroy (GTK_WIDGET (w));

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect (
    G_OBJECT (self->language_cb), "changed",
    G_CALLBACK (on_language_changed), self);
  g_signal_connect (
    G_OBJECT (self->audio_backend), "changed",
    G_CALLBACK (on_audio_backend_changed), self);
  g_signal_connect (
    G_OBJECT (self->midi_backend), "changed",
    G_CALLBACK (on_midi_backend_changed), self);
  g_signal_connect (
    G_OBJECT (self->language_cb), "changed",
    G_CALLBACK (on_language_changed), self);
  g_signal_connect (
    G_OBJECT (self->fc_btn), "file-set",
    G_CALLBACK (on_file_set), self);
  g_signal_connect (
    G_OBJECT (self->reset), "clicked",
    G_CALLBACK (on_reset_clicked), self);
  g_signal_connect (
    G_OBJECT (self->test_backends), "clicked",
    G_CALLBACK (on_test_backends_clicked), self);
}
