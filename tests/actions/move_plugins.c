/*
 * Copyright (C) 2020-2021 Alexandros Theodotou <alex at zrythm dot org>
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

#include "zrythm-test-config.h"

#include "actions/move_plugins_action.h"
#include "actions/undoable_action.h"
#include "actions/undo_manager.h"
#include "audio/audio_region.h"
#include "audio/automation_region.h"
#include "audio/chord_region.h"
#include "audio/control_port.h"
#include "audio/master_track.h"
#include "audio/midi_note.h"
#include "audio/region.h"
#include "plugins/plugin_manager.h"
#include "project.h"
#include "utils/flags.h"
#include "zrythm.h"

#include "tests/helpers/plugin_manager.h"
#include "tests/helpers/project.h"

#include <glib.h>

#ifdef HAVE_LSP_COMPRESSOR
static void
_test_port_and_plugin_track_pos_after_move (
  const char * pl_bundle,
  const char * pl_uri,
  bool         with_carla)
{
  PluginDescriptor * descr =
    test_plugin_manager_get_plugin_descriptor (
      pl_bundle, pl_uri, with_carla);

  /* create an instrument track from helm */
  UndoableAction * ua =
    create_tracks_action_new (
      TRACK_TYPE_AUDIO_BUS, descr, NULL,
      TRACKLIST->num_tracks, NULL, 1);
  undo_manager_perform (UNDO_MANAGER, ua);

  plugin_descriptor_free (descr);

  int src_track_pos = TRACKLIST->num_tracks - 1;
  int dest_track_pos = TRACKLIST->num_tracks;

  /* select it */
  Track * src_track =
    TRACKLIST->tracks[src_track_pos];
  track_select (
    src_track, F_SELECT, true, F_NO_PUBLISH_EVENTS);

  /* get an automation track */
  AutomationTracklist * atl =
    track_get_automation_tracklist (src_track);
  AutomationTrack * at = atl->ats[atl->num_ats - 1];
  at->created = true;
  at->visible = true;

  /* create an automation region */
  Position start_pos, end_pos;
  position_set_to_bar (&start_pos, 2);
  position_set_to_bar (&end_pos, 4);
  ZRegion * region =
    automation_region_new (
      &start_pos, &end_pos, src_track->pos,
      at->index, at->num_regions);
  track_add_region  (
    src_track, region, at, -1, F_GEN_NAME,
    F_NO_PUBLISH_EVENTS);
  arranger_object_select (
    (ArrangerObject *) region, true, false);
  ua =
    arranger_selections_action_new_create (
      TL_SELECTIONS);
  undo_manager_perform (UNDO_MANAGER, ua);

  /* create some automation points */
  Port * port = automation_track_get_port (at);
  position_set_to_bar (&start_pos, 1);
  AutomationPoint * ap =
    automation_point_new_float (
      port->deff,
      control_port_real_val_to_normalized (
        port, port->deff),
      &start_pos);
  automation_region_add_ap (
    region, ap, F_NO_PUBLISH_EVENTS);
  arranger_object_select (
    (ArrangerObject *) ap, true, false);
  ua =
    arranger_selections_action_new_create (
      AUTOMATION_SELECTIONS);
  undo_manager_perform (UNDO_MANAGER, ua);

  /* duplicate it */
  ua =
    copy_tracks_action_new (
      TRACKLIST_SELECTIONS, TRACKLIST->num_tracks);

  g_assert_true (
    track_verify_identifiers (src_track));

  undo_manager_perform (UNDO_MANAGER, ua);

  Track * dest_track =
    TRACKLIST->tracks[dest_track_pos];

  g_assert_true (
    track_verify_identifiers (src_track));
  g_assert_true (
    track_verify_identifiers (dest_track));

  /* move plugin from 1st track to 2nd track and
   * undo/redo */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, src_track,
    PLUGIN_SLOT_INSERT, 0);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      dest_track, 1);

  undo_manager_perform (UNDO_MANAGER, ua);

  /* let the engine run */
  g_usleep (1000000);

  g_assert_true (
    track_verify_identifiers (src_track));
  g_assert_true (
    track_verify_identifiers (dest_track));

  undo_manager_undo (UNDO_MANAGER);

  g_assert_true (
    track_verify_identifiers (src_track));
  g_assert_true (
    track_verify_identifiers (dest_track));

  undo_manager_redo (UNDO_MANAGER);

  g_assert_true (
    track_verify_identifiers (src_track));
  g_assert_true (
    track_verify_identifiers (dest_track));

  undo_manager_undo (UNDO_MANAGER);

  /* move plugin from 1st slot to the 2nd slot and
   * undo/redo */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, src_track,
    PLUGIN_SLOT_INSERT, 0);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      src_track, 1);
  undo_manager_perform (UNDO_MANAGER, ua);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_redo (UNDO_MANAGER);

  /* let the engine run */
  g_usleep (1000000);

  /* move the plugin to a new track */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  src_track = TRACKLIST->tracks[src_track_pos];
  mixer_selections_add_slot (
    MIXER_SELECTIONS, src_track,
    PLUGIN_SLOT_INSERT, 1);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      NULL, 0);
  undo_manager_perform (UNDO_MANAGER, ua);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_redo (UNDO_MANAGER);

  /* let the engine run */
  g_usleep (1000000);

  /* go back to the start */
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
}
#endif

static void
test_port_and_plugin_track_pos_after_move (void)
{
  test_helper_zrythm_init ();

#ifdef HAVE_LSP_COMPRESSOR
  _test_port_and_plugin_track_pos_after_move (
    LSP_COMPRESSOR_BUNDLE, LSP_COMPRESSOR_URI,
    false);
#endif

  test_helper_zrythm_cleanup ();
}

#ifdef HAVE_CARLA
static void
test_port_and_plugin_track_pos_after_move_with_carla (void)
{
  test_helper_zrythm_init ();

#ifdef HAVE_LSP_COMPRESSOR
  _test_port_and_plugin_track_pos_after_move (
    LSP_COMPRESSOR_BUNDLE, LSP_COMPRESSOR_URI,
    true);
#endif

  test_helper_zrythm_cleanup ();
}
#endif

static void
test_move_two_plugins_one_slot_up (void)
{
  test_helper_zrythm_init ();

#ifdef HAVE_LSP_COMPRESSOR
  /* create a track with an insert */
  PluginDescriptor * descr =
    test_plugin_manager_get_plugin_descriptor (
      LSP_COMPRESSOR_BUNDLE,
      LSP_COMPRESSOR_URI, false);
  UndoableAction * ua =
    create_tracks_action_new (
      TRACK_TYPE_AUDIO_BUS, descr, NULL,
      TRACKLIST->num_tracks, NULL, 1);
  undo_manager_perform (UNDO_MANAGER, ua);
  plugin_descriptor_free (descr);

  int track_pos = TRACKLIST->num_tracks - 1;

  /* select it */
  Track * track =
    TRACKLIST->tracks[track_pos];
  track_select (
    track, F_SELECT, true, F_NO_PUBLISH_EVENTS);

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* get an automation track */
  AutomationTracklist * atl =
    track_get_automation_tracklist (track);
  AutomationTrack * at = atl->ats[atl->num_ats - 1];
  g_message (
    "automation track %s", at->port_id.label);
  at->created = true;
  at->visible = true;

  /* create an automation region */
  Position start_pos, end_pos;
  position_set_to_bar (&start_pos, 2);
  position_set_to_bar (&end_pos, 4);
  ZRegion * region =
    automation_region_new (
      &start_pos, &end_pos, track->pos,
      at->index, at->num_regions);
  track_add_region  (
    track, region, at, -1, F_GEN_NAME,
    F_NO_PUBLISH_EVENTS);
  arranger_object_select (
    (ArrangerObject *) region, true, false);
  ua =
    arranger_selections_action_new_create (
      TL_SELECTIONS);
  undo_manager_perform (UNDO_MANAGER, ua);

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* create some automation points */
  Port * port = automation_track_get_port (at);
  position_set_to_bar (&start_pos, 1);
  atl = track_get_automation_tracklist (track);
  at = atl->ats[atl->num_ats - 1];
  g_assert_cmpint (at->num_regions, >, 0);
  region = at->regions[0];
  AutomationPoint * ap =
    automation_point_new_float (
      port->deff,
      control_port_real_val_to_normalized (
        port, port->deff),
      &start_pos);
  automation_region_add_ap (
    region, ap, F_NO_PUBLISH_EVENTS);
  arranger_object_select (
    (ArrangerObject *) ap, true, false);
  ua =
    arranger_selections_action_new_create (
      AUTOMATION_SELECTIONS);
  undo_manager_perform (UNDO_MANAGER, ua);

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* duplicate the plugin to the 2nd slot */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 0);
  ua =
    copy_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      track, 1);
  undo_manager_perform (UNDO_MANAGER, ua);

  /* at this point we have a plugin at slot#0 and
   * its clone at slot#1 */

  /* remove slot #0 and undo */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 0);
  ua =
    delete_plugins_action_new (MIXER_SELECTIONS);
  undo_manager_perform (UNDO_MANAGER, ua);
  undo_manager_undo (UNDO_MANAGER);

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* move the 2 plugins to start at slot#1 (2nd
   * slot) */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 0);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 1);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      track, 1);
  undo_manager_perform (UNDO_MANAGER, ua);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_undo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_redo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* move the 2 plugins to start at slot 2 (3rd
   * slot) */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 1);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 2);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      track, 2);
  undo_manager_perform (UNDO_MANAGER, ua);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_undo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_redo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* move the 2 plugins to start at slot 1 (2nd
   * slot) */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 2);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 3);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      track, 1);
  undo_manager_perform (UNDO_MANAGER, ua);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_undo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));
  undo_manager_redo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));

  /* save and reload the project */
  test_project_save_and_reload ();
  track = TRACKLIST->tracks[track_pos];
  g_assert_true (track_verify_identifiers (track));

  /* move the 2 plugins to start back at slot 0 (1st
   * slot) */
  mixer_selections_clear (
    MIXER_SELECTIONS, F_NO_PUBLISH_EVENTS);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 2);
  mixer_selections_add_slot (
    MIXER_SELECTIONS, track,
    PLUGIN_SLOT_INSERT, 1);
  ua =
    move_plugins_action_new (
      MIXER_SELECTIONS, PLUGIN_SLOT_INSERT,
      track, 0);
  undo_manager_perform (UNDO_MANAGER, ua);

  g_assert_true (
    track_verify_identifiers (track));

  /* let the engine run */
  g_usleep (1000000);

  g_assert_true (
    track_verify_identifiers (track));

  undo_manager_undo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));

  undo_manager_redo (UNDO_MANAGER);
  g_assert_true (
    track_verify_identifiers (track));

  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
  undo_manager_undo (UNDO_MANAGER);
#endif

  test_helper_zrythm_cleanup ();
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

#define TEST_PREFIX "/actions/move_plugins/"

  g_test_add_func (
    TEST_PREFIX
    "test port and plugin track pos after move",
    (GTestFunc)
    test_port_and_plugin_track_pos_after_move);
#ifdef HAVE_CARLA
  g_test_add_func (
    TEST_PREFIX
    "test port and plugin track pos after move with carla",
    (GTestFunc)
    test_port_and_plugin_track_pos_after_move_with_carla);
#endif
  g_test_add_func (
    TEST_PREFIX
    "test move two plugins one slot up",
    (GTestFunc)
    test_move_two_plugins_one_slot_up);

  return g_test_run ();
}
