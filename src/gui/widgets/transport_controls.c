/*
 * Copyright (C) 2018-2020 Alexandros Theodotou <alex at zrythm dot org>
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
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "project.h"
#include "audio/transport.h"
#include "audio/midi.h"
#include "gui/backend/event.h"
#include "gui/backend/event_manager.h"
#include "gui/widgets/button_with_menu.h"
#include "gui/widgets/transport_controls.h"
#include "settings/settings.h"
#include "utils/flags.h"
#include "utils/gtk.h"
#include "utils/resources.h"
#include "utils/string.h"
#include "zrythm_app.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>

G_DEFINE_TYPE (
  TransportControlsWidget,
  transport_controls_widget,
  GTK_TYPE_BOX)

static void
play_clicked_cb (
  GtkButton * button,
  gpointer    user_data)
{
  if (TRANSPORT->play_state == PLAYSTATE_ROLLING)
    {
      position_set_to_pos (
        &TRANSPORT->playhead_pos,
        &TRANSPORT->cue_pos);
    }
  else
    {
      transport_request_roll (TRANSPORT);
    }
}

static void
stop_clicked_cb (GtkButton *button,
               gpointer       user_data)
{
  /*g_message ("playstate %d", TRANSPORT->play_state);*/
  if (TRANSPORT->play_state == PLAYSTATE_PAUSED)
    {
      transport_set_playhead_pos (
        TRANSPORT, &TRANSPORT->cue_pos);
    }
  else
    transport_request_pause (TRANSPORT);

  midi_panic_all (1);
}

static void
record_toggled_cb (
  GtkToggleButton * tg,
  gpointer        user_data)
{
  if (gtk_toggle_button_get_active (tg))
    {
      TRANSPORT->recording = 1;
    }
  else
    {
      TRANSPORT->recording = 0;
    }
}

static void
forward_clicked_cb (GtkButton * forward,
                    gpointer          user_data)
{
  Position *pos =
    snap_grid_get_nearby_snap_point (
      &PROJECT->snap_grid_timeline,
      &TRANSPORT->playhead_pos, 0);
  transport_move_playhead (
    TRANSPORT, pos, F_PANIC, F_SET_CUE_POINT);
}

static void
backward_clicked_cb (GtkButton * backward,
                     gpointer    user_data)
{
  Position * pos =
    snap_grid_get_nearby_snap_point (
      &PROJECT->snap_grid_timeline,
      &TRANSPORT->playhead_pos, 1);
  transport_move_playhead (
    TRANSPORT, pos, F_PANIC, F_SET_CUE_POINT);
}

static void
change_state_punch_mode (
  GSimpleAction *action,
  GVariant      *variant,
  gpointer       user_data)
{
  bool value = g_variant_get_boolean (variant);
  transport_set_punch_mode_enabled (
    TRANSPORT, value);
  g_message ("setting punch mode to %d", value);
  g_simple_action_set_state (action, variant);

  EVENTS_PUSH (
    ET_TIMELINE_PUNCH_MARKER_POS_CHANGED, NULL);
}

static void
change_start_on_midi_input (
  GSimpleAction *action,
  GVariant      *variant,
  gpointer       user_data)
{
  bool value = g_variant_get_boolean (variant);
  transport_set_start_playback_on_midi_input (
    TRANSPORT, value);
  g_message (
    "setting start on MIDI input to %d", value);
  g_simple_action_set_state (action, variant);
}

static void
activate_recording_mode (
  GSimpleAction *action,
  GVariant      *_variant,
  gpointer       user_data)
{
  g_return_if_fail (_variant);

  gsize size;
  const char * variant =
    g_variant_get_string (_variant, &size);
  g_simple_action_set_state (action, _variant);
  if (string_is_equal (variant, "overwrite"))
    {
      transport_set_recording_mode (
        TRANSPORT, RECORDING_MODE_OVERWRITE_EVENTS);
    }
  if (string_is_equal (variant, "merge"))
    {
      transport_set_recording_mode (
        TRANSPORT, RECORDING_MODE_MERGE_EVENTS);
    }
  else if (string_is_equal (variant, "takes"))
    {
      transport_set_recording_mode (
        TRANSPORT, RECORDING_MODE_TAKES);
    }
  else if (string_is_equal (variant, "takes-muted"))
    {
      transport_set_recording_mode (
        TRANSPORT, RECORDING_MODE_TAKES_MUTED);
    }
  g_message ("recording mode changed");
}

static void
setup_record_btn (
  TransportControlsWidget * self)
{
  /* create main button */
  self->trans_record_btn =
    z_gtk_toggle_button_new_with_icon (
      "media-record");
  z_gtk_widget_add_style_class (
    GTK_WIDGET (self->trans_record_btn),
    "record-button");
  gtk_widget_set_size_request (
    GTK_WIDGET (self->trans_record_btn), 20, -1);

  /* set menu */
  GMenu * menu = g_menu_new ();
  GMenu * punch_section = g_menu_new ();
  g_menu_append (
    punch_section, _("Punch in/out"),
    "record-btn.punch-mode");
  g_menu_append (
    punch_section, _("Start on MIDI input"),
    "record-btn.start-on-midi-input");
  g_menu_append_section (
    menu, _("Options"),
    G_MENU_MODEL (punch_section));
  g_object_unref (punch_section);
  GMenu * modes_section = g_menu_new ();
  g_menu_append (
    modes_section, _("Overwrite events"),
    "record-btn.recording-mode::overwrite");
  g_menu_append (
    modes_section, _("Merge events"),
    "record-btn.recording-mode::merge");
  g_menu_append (
    modes_section, _("Create takes"),
    "record-btn.recording-mode::takes");
  g_menu_append (
    modes_section, _("Create takes (mute previous)"),
    "record-btn.recording-mode::takes-muted");
  g_menu_append_section (
    menu, _("Recording mode"),
    G_MENU_MODEL (modes_section));
  g_object_unref (modes_section);
  GSimpleActionGroup * action_group =
    g_simple_action_group_new ();
  const char * recording_modes[] = {
    "'overwrite'", "'merge'", "'takes'",
    "'takes-muted'", };
  GActionEntry actions[] = {
    { "punch-mode", NULL, NULL,
      (TRANSPORT->punch_mode ? "true" : "false"),
      change_state_punch_mode },
    { "start-on-midi-input", NULL, NULL,
      (TRANSPORT->start_playback_on_midi_input ?
         "true" : "false"),
      change_start_on_midi_input },
    { "recording-mode",
      activate_recording_mode, "s",
      recording_modes[TRANSPORT->recording_mode] },
  };
  g_action_map_add_action_entries (
    G_ACTION_MAP (action_group), actions,
    G_N_ELEMENTS (actions), self);
  gtk_widget_insert_action_group (
    GTK_WIDGET (self->trans_record), "record-btn",
    G_ACTION_GROUP (action_group));

  /* setup button with menu widget */
  button_with_menu_widget_setup (
    self->trans_record,
    GTK_BUTTON (self->trans_record_btn),
    NULL, G_MENU_MODEL (menu),
    false, 38, _("Record"),
    _("Record options"));
}

static void
transport_controls_widget_class_init (
  TransportControlsWidgetClass * _klass)
{
  GtkWidgetClass * klass = GTK_WIDGET_CLASS (_klass);
  resources_set_class_template (
    klass, "transport_controls.ui");

  gtk_widget_class_set_css_name (
    klass, "transport-controls");

#define BIND_CHILD(x) \
  gtk_widget_class_bind_template_child ( \
    klass, TransportControlsWidget, x)

  BIND_CHILD (play);
  BIND_CHILD (stop);
  BIND_CHILD (backward);
  BIND_CHILD (forward);
  BIND_CHILD (trans_record);
  BIND_CHILD (loop);
  BIND_CHILD (return_to_cue_toggle);

#undef BIND_CHILD
}

static void
transport_controls_widget_init (
  TransportControlsWidget * self)
{
  g_type_ensure (BUTTON_WITH_MENU_WIDGET_TYPE);

  gtk_widget_init_template (GTK_WIDGET (self));

  /* setup record button */
  setup_record_btn (self);

  /* make play button bigger */
  gtk_widget_set_size_request (
    GTK_WIDGET (self->play), 30, -1);

  gtk_toggle_button_set_active (
    self->loop,
    g_settings_get_boolean (S_TRANSPORT, "loop"));

  g_settings_bind (
    S_TRANSPORT, "return-to-cue",
    self->return_to_cue_toggle, "active",
    G_SETTINGS_BIND_DEFAULT);

  g_signal_connect (
    GTK_WIDGET (self->play), "clicked",
    G_CALLBACK (play_clicked_cb), NULL);
  g_signal_connect (
    GTK_WIDGET (self->stop), "clicked",
    G_CALLBACK (stop_clicked_cb), NULL);
  g_signal_connect (
    GTK_WIDGET (self->trans_record_btn), "toggled",
    G_CALLBACK (record_toggled_cb), NULL);
  g_signal_connect (
    GTK_WIDGET(self->forward), "clicked",
    G_CALLBACK(forward_clicked_cb), NULL);
  g_signal_connect (
    GTK_WIDGET(self->backward), "clicked",
    G_CALLBACK(backward_clicked_cb), NULL);
}
