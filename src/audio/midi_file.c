/*
 * Copyright (C) 2020 Alexandros Theodotou <alex at zrythm dot org>
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

#include "audio/midi_file.h"

#include <ext/midilib/src/midifile.h>

#include <gtk/gtk.h>

/**
 * Returns the number of tracks in the MIDI file.
 */
int
midi_file_get_num_tracks (
  const char * abs_path,
  bool         non_empty_only)
{
  MIDI_FILE * mf =
    midiFileOpen (abs_path);
  g_return_val_if_fail (mf, -1);

  int num = midiReadGetNumTracks (mf);
  g_debug ("%s: num tracks = %d", abs_path, num);
  midiFileClose (mf);

  return num;
}
