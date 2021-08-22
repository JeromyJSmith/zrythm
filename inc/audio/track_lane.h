/*
 * Copyright (C) 2019-2021 Alexandros Theodotou <alex at zrythm dot org>
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

/**
 * \file
 *
 * Track lanes for each track.
 */

#ifndef __AUDIO_TRACK_LANE_H__
#define __AUDIO_TRACK_LANE_H__

#include "audio/region.h"
#include "utils/yaml.h"

typedef struct _TrackLaneWidget TrackLaneWidget;
typedef struct Tracklist Tracklist;
typedef struct CustomButtonWidget
  CustomButtonWidget;
typedef void MIDI_FILE;

/**
 * @addtogroup audio
 *
 * @{
 */
#define MAX_REGIONS 300

/**
 * A TrackLane belongs to a Track (can have many
 * TrackLanes in a Track) and contains Regions.
 *
 * Only Tracks that have Regions can have TrackLanes,
 * such as InstrumentTrack and AudioTrack.
 */
typedef struct TrackLane
{
  /** Position in the Track. */
  int                 pos;

  /** Name of lane, e.g. "Lane 1". */
  char *              name;

  /** TrackLaneWidget for this lane. */
  //TrackLaneWidget *   widget;

  /** Y local to track. */
  int                 y;

  /** Position of handle. */
  double              height;

  /** Muted or not. */
  int                 mute;

  /** Soloed or not. */
  int                 solo;

  /** Regions in this track. */
  ZRegion **          regions;
  int                 num_regions;
  size_t              regions_size;

  /**
   * MIDI channel, if MIDI lane, starting at 1.
   *
   * If this is set to 0, the value will be
   * inherited from the Track.
   */
  uint8_t             midi_ch;

  /** Buttons used by the track widget. */
  CustomButtonWidget * buttons[8];
  int                 num_buttons;

  /** Whether part of an auditioner track. */
  bool                is_auditioner;

  /** Owner track. */
  Track *             track;

} TrackLane;

static const cyaml_schema_field_t
track_lane_fields_schema[] =
{
  YAML_FIELD_INT (
    TrackLane, pos),
  YAML_FIELD_STRING_PTR (
    TrackLane, name),
  YAML_FIELD_FLOAT (
    TrackLane, height),
  YAML_FIELD_INT (
    TrackLane, mute),
  YAML_FIELD_INT (
    TrackLane, solo),
  YAML_FIELD_DYN_ARRAY_VAR_COUNT (
    TrackLane, regions, region_schema),
  YAML_FIELD_UINT (
    TrackLane, midi_ch),

  CYAML_FIELD_END
};

static const cyaml_schema_value_t
track_lane_schema = {
  CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER,
    TrackLane, track_lane_fields_schema),
};

void
track_lane_init_loaded (
  TrackLane * lane);

/**
 * Creates a new TrackLane at the given pos in the
 * given Track.
 *
 * @param track The Track to create the TrackLane
 *   for.
 * @param pos The position (index) in the Track that
 *   this lane will be placed in.
 */
TrackLane *
track_lane_new (
  Track * track,
  int     pos);

/**
 * Inserts a ZRegion to the given TrackLane at the
 * given index.
 */
void
track_lane_insert_region (
  TrackLane * self,
  ZRegion *   region,
  int         idx);

/**
 * Adds a ZRegion to the given TrackLane.
 */
void
track_lane_add_region (
  TrackLane * self,
  ZRegion *   region);

/**
 * Removes but does not free the region.
 */
void
track_lane_remove_region (
  TrackLane * self,
  ZRegion *   region);

/**
 * Unselects all arranger objects.
 */
void
track_lane_unselect_all (
  TrackLane * self);

/**
 * Removes all objects recursively from the track
 * lane.
 */
void
track_lane_clear (
  TrackLane * self);

/**
 * Rename the lane.
 *
 * @param with_action Whether to make this an
 *   undoable action.
 */
void
track_lane_rename (
  TrackLane *  self,
  const char * new_name,
  bool         with_action);

/**
 * Wrapper over track_lane_rename().
 */
void
track_lane_rename_with_action (
  TrackLane *  self,
  const char * new_name);

const char *
track_lane_get_name (
  TrackLane * self);

/**
 * Updates the frames of each position in each child
 * of the track recursively.
 */
void
track_lane_update_frames (
  TrackLane * self);

/**
 * Sets the new track name hash to all the lane's
 * objects recursively.
 */
void
track_lane_update_track_name_hash (
  TrackLane *   self);

/**
 * Clones the TrackLane.
 *
 * Mainly used when cloning Track's.
 *
 * @param track Pointer to owner track.
 */
TrackLane *
track_lane_clone (
  const TrackLane * src,
  Track *           track);

/**
 * Writes the lane to the given MIDI file.
 */
void
track_lane_write_to_midi_file (
  TrackLane * self,
  MIDI_FILE * mf);

NONNULL
Tracklist *
track_lane_get_tracklist (
  TrackLane * self);

NONNULL
Track *
track_lane_get_track (
  TrackLane * self);

/**
 * Frees the TrackLane.
 */
NONNULL
void
track_lane_free (
  TrackLane * lane);

/**
 * @}
 */

#endif // __AUDIO_TRACK_LANE_H__
