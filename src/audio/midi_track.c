// SPDX-License-Identifier: AGPL-3.0-or-later
/*
 * Copyright (C) 2018-2021 Alexandros Theodotou <alex at zrythm dot org>
 */

#include "zrythm-config.h"

#include <inttypes.h>
#include <stdlib.h>

#include "audio/automation_track.h"
#include "audio/automation_tracklist.h"
#include "audio/midi_event.h"
#include "audio/midi_note.h"
#include "audio/midi_track.h"
#include "audio/position.h"
#include "audio/region.h"
#include "audio/track.h"
#include "audio/velocity.h"
#include "gui/widgets/track.h"
#include "project.h"
#include "utils/arrays.h"
#include "utils/flags.h"
#include "utils/math.h"
#include "utils/stoat.h"

#include <gtk/gtk.h>

/**
 * Initializes an midi track.
 */
void
midi_track_init (Track * self)
{
  self->type = TRACK_TYPE_MIDI;
  gdk_rgba_parse (&self->color, "#F79616");

  self->icon_name = g_strdup ("signal-midi");
}

void
midi_track_setup (Track * self)
{
  channel_track_setup (self);
}

/**
 * Frees the track.
 *
 * TODO
 */
void
midi_track_free (Track * track)
{
}
