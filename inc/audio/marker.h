/*
 * Copyright (C) 2019-2020 Alexandros Theodotou <alex at zrythm dot org>
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
 * Marker related code.
 */

#ifndef __AUDIO_MARKER_H__
#define __AUDIO_MARKER_H__

#include <stdint.h>

#include "audio/position.h"
#include "gui/backend/arranger_object.h"
#include "utils/yaml.h"

/**
 * @addtogroup audio
 *
 * @{
 */

#define MARKER_WIDGET_TRIANGLE_W 10

#define marker_is_selected(r) \
  arranger_object_is_selected ( \
    (ArrangerObject *) r)

#define marker_is_deletable(m) \
  ((m)->type != MARKER_TYPE_START && \
   (m)->type != MARKER_TYPE_END)

/**
 * Marker type.
 */
typedef enum MarkerType
{
  /** Song start Marker that cannot be deleted. */
  MARKER_TYPE_START,
  /** Song end Marker that cannot be deleted. */
  MARKER_TYPE_END,
  /** Custom Marker. */
  MARKER_TYPE_CUSTOM,
} MarkerType;

static const cyaml_strval_t
marker_type_strings[] =
{
  { "start",     MARKER_TYPE_START    },
  { "end",       MARKER_TYPE_END   },
  { "custom",    MARKER_TYPE_CUSTOM   },
};

/**
 * Marker for the MarkerTrack.
 */
typedef struct Marker
{
  /** Base struct. */
  ArrangerObject  base;

  /** Marker type. */
  MarkerType        type;

  /** Name of Marker to be displayed in the UI. */
  char *            name;

  /** Position of the marker track this marker is
   * in. */
  int               track_pos;

  /** Index in the track. */
  int               index;

  /** Cache layout for drawing the name. */
  PangoLayout *      layout;
} Marker;

static const cyaml_schema_field_t
  marker_fields_schema[] =
{
  CYAML_FIELD_MAPPING (
    "base", CYAML_FLAG_DEFAULT,
    Marker, base,
    arranger_object_fields_schema),
  CYAML_FIELD_STRING_PTR (
    "name", CYAML_FLAG_POINTER,
    Marker, name,
    0, CYAML_UNLIMITED),
  CYAML_FIELD_INT (
    "track_pos", CYAML_FLAG_DEFAULT,
    Marker, track_pos),
  CYAML_FIELD_INT (
    "index", CYAML_FLAG_DEFAULT,
    Marker, index),
  CYAML_FIELD_ENUM (
    "type", CYAML_FLAG_DEFAULT,
    Marker, type, marker_type_strings,
    CYAML_ARRAY_LEN (marker_type_strings)),

  CYAML_FIELD_END
};

static const cyaml_schema_value_t
marker_schema = {
  CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER,
    Marker, marker_fields_schema),
};

/**
 * Creates a Marker.
 */
Marker *
marker_new (
  const char * name);

/**
 * Returns if the two Marker's are equal.
 */
int
marker_is_equal (
  Marker * a,
  Marker * b);

/**
 * Sets the Track of the Marker.
 */
void
marker_set_track (
  Marker * marker,
  Track *  track);

Marker *
marker_find_by_name (
  const char * name);

/**
 * Sets the name to all the Marker's counterparts.
 */
#define marker_set_name(_self,_name) \
  arranger_object_set_string ( \
    Marker, _self, name, _name)

/**
 * @}
 */

#endif
