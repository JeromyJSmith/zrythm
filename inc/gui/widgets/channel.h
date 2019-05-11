/*
 * Copyright (C) 2018-2019 Alexandros Theodotou
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

#ifndef __GUI_WIDGETS_CHANNEL_H__
#define __GUI_WIDGETS_CHANNEL_H__

#include "audio/channel.h"
#include "gui/widgets/meter.h"

#include <gtk/gtk.h>

typedef struct _ExpanderBoxWidget ExpanderBoxWidget;

#define CHANNEL_WIDGET_TYPE \
  (channel_widget_get_type ())
G_DECLARE_FINAL_TYPE (ChannelWidget,
                      channel_widget,
                      Z,
                      CHANNEL_WIDGET,
                      GtkEventBox)

typedef struct _ColorAreaWidget ColorAreaWidget;
typedef struct _KnobWidget KnobWidget;
typedef struct _FaderWidget FaderWidget;
typedef struct _ChannelMeterWidget ChannelMeterWidget;
typedef struct Channel Channel;
typedef struct _ChannelSlotWidget ChannelSlotWidget;
typedef struct _RouteTargetSelectorWidget
  RouteTargetSelectorWidget;
typedef struct _PanWidget PanWidget;
typedef struct _EditableLabelWidget
  EditableLabelWidget;

typedef struct _ChannelWidget
{
  GtkEventBox         parent_instance;
  GtkGrid *          grid;
  RouteTargetSelectorWidget * output;
  ColorAreaWidget     * color;
  GtkEventBox *       icon_and_name_event_box;
  EditableLabelWidget * name;
  GtkBox              * phase_controls;
  GtkButton           * phase_invert;
  GtkLabel            * phase_reading;
  KnobWidget          * phase_knob;

  /* ----- Inserts ------ */
  ExpanderBoxWidget * inserts_expander;
  GtkBox              * inserts_box;
  GtkBox              * insert_boxes[STRIP_SIZE];      ///< array of slot boxes (1 per plugin)
  ChannelSlotWidget   * inserts[STRIP_SIZE];
  GtkToggleButton     * insert_toggles[STRIP_SIZE];   ///< toggle buttons (per slot)
  GtkLabel            * insert_labels[STRIP_SIZE];     ///< labels (per slot)
  GtkToggleButton     * slot1b;
  GtkToggleButton     * slot2b;

  /* -------- end --------- */

  GtkButton           * e;
  GtkToggleButton *      solo;
  GtkToggleButton           * listen;
  GtkToggleButton           * mute;
  GtkToggleButton     * record;
  GtkBox              * meter_area;  ///< vertical including reading
  GtkBox *            pan_box;
  PanWidget *         pan;
  FaderWidget         * fader;
  MeterWidget         * meter_l;
  MeterWidget         * meter_r;
  GtkLabel            * meter_reading;
  GtkImage            * icon;
  double                meter_reading_val; ///< cache

  /** Used for highlighting. */
  GtkBox *            highlight_left_box;
  GtkBox *            highlight_right_box;

  /** Number of clicks, used when selecting/moving/
   * dragging channels. */
  int                 n_press;

  /** Control held down on drag begin. */
  int                 ctrl_held_at_start;

  /** If drag update was called at least once. */
  int                 dragged;

  /** The track selection processing was done in
   * the dnd callbacks, so no need to do it in
   * drag_end. */
  int                 selected_in_dnd;

  /** Pointer to owner Channel. */
  Channel             * channel;

  /**
   * Signal handler IDs.
   */
  gulong              record_toggled_handler_id;
  gulong              solo_toggled_handler_id;
  gulong              mute_toggled_handler_id;

  /** Whole channel press. */
  GtkGestureMultiPress * mp;

  /** Drag on the icon and name event box. */
  GtkGestureDrag       * drag;
} ChannelWidget;

/**
 * Updates the inserts.
 */
void
channel_widget_update_inserts (
  ChannelWidget * self);

/**
 * Blocks all signal handlers.
 */
void
channel_widget_block_all_signal_handlers (
  ChannelWidget * self);

/**
 * Unblocks all signal handlers.
 */
void
channel_widget_unblock_all_signal_handlers (
  ChannelWidget * self);

/**
 * Creates a channel widget using the given channel data.
 */
ChannelWidget *
channel_widget_new (Channel * channel);

/**
 * Updates the meter reading
 */
gboolean
channel_widget_update_meter_reading (
  ChannelWidget * widget,
  GdkFrameClock * frame_clock,
  gpointer        user_data);

/**
 * Updates everything on the widget.
 *
 * It is reduntant but keeps code organized. Should fix if it causes lags.
 */
void
channel_widget_refresh (ChannelWidget * self);

void
channel_widget_refresh_buttons (
  ChannelWidget * self);

/**
 * Displays the widget.
 */
void
channel_widget_show (ChannelWidget * self);


#endif
