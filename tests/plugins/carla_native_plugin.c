/*
 * Copyright (C) 2021 Alexandros Theodotou <alex at zrythm dot org>
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

#include "audio/fader.h"
#include "audio/router.h"
#include "plugins/carla_native_plugin.h"
#include "plugins/carla/carla_discovery.h"
#include "utils/math.h"

#include "tests/helpers/plugin_manager.h"
#include "tests/helpers/zrythm.h"

#include <glib.h>

#if 0
static void
test_has_custom_ui (void)
{
  test_helper_zrythm_init ();

#ifdef HAVE_CARLA
#ifdef HAVE_HELM
  PluginSetting * setting =
    test_plugin_manager_get_plugin_setting (
      HELM_BUNDLE, HELM_URI, false);
  g_assert_nonnull (setting);
  g_assert_true (
    carla_native_plugin_has_custom_ui (
      setting->descr));
#endif
#endif

  test_helper_zrythm_cleanup ();
}
#endif

static void
test_crash_handling (void)
{
#ifdef HAVE_CARLA
  test_helper_zrythm_init ();

  /* stop dummy audio engine processing so we can
   * process manually */
  AUDIO_ENGINE->stop_dummy_audio_thread = true;
  g_usleep (1000000);

  PluginSetting * setting =
    test_plugin_manager_get_plugin_setting (
    SIGABRT_BUNDLE_URI, SIGABRT_URI, true);
  g_assert_nonnull (setting);
  setting->bridge_mode = CARLA_BRIDGE_FULL;

  /* create a track from the plugin */
  track_create_for_plugin_at_idx_w_action (
    TRACK_TYPE_AUDIO_BUS, setting,
    TRACKLIST->num_tracks, NULL);

  Plugin * pl =
    TRACKLIST->tracks[TRACKLIST->num_tracks - 1]->
      channel->inserts[0];
  g_assert_true (IS_PLUGIN_AND_NONNULL (pl));

  engine_process (
    AUDIO_ENGINE, AUDIO_ENGINE->block_length);
  engine_process (
    AUDIO_ENGINE, AUDIO_ENGINE->block_length);
  engine_process (
    AUDIO_ENGINE, AUDIO_ENGINE->block_length);

  test_helper_zrythm_cleanup ();
#endif
}

/**
 * Test process.
 */
static void
test_process (void)
{
#if defined (HAVE_TEST_SIGNAL) && \
  defined (HAVE_CARLA)

  test_helper_zrythm_init ();

  test_plugin_manager_create_tracks_from_plugin (
    TEST_SIGNAL_BUNDLE, TEST_SIGNAL_URI, false,
    true, 1);

  Track * track =
    TRACKLIST->tracks[TRACKLIST->num_tracks - 1];
  Plugin * pl = track->channel->inserts[0];
  g_assert_true (IS_PLUGIN_AND_NONNULL (pl));

  /* stop dummy audio engine processing so we can
   * process manually */
  AUDIO_ENGINE->stop_dummy_audio_thread = true;
  g_usleep (1000000);

  /* run plugin and check that output is filled */
  Port * out = pl->out_ports[0];
  nframes_t local_offset = 60;
  carla_native_plugin_process (
    pl->carla, 0,  0, local_offset);
  for (nframes_t i = 1; i < local_offset; i++)
    {
      g_assert_true (fabsf (out->buf[i]) > 1e-10f);
    }
  carla_native_plugin_process (
    pl->carla, local_offset,
    local_offset,
    AUDIO_ENGINE->block_length - local_offset);
  for (nframes_t i = local_offset;
       i < AUDIO_ENGINE->block_length; i++)
    {
      g_assert_true (fabsf (out->buf[i]) > 1e-10f);
    }

  test_helper_zrythm_cleanup ();
#endif
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

#define TEST_PREFIX "/plugins/carla native plugin/"

  g_test_add_func (
    TEST_PREFIX "test process",
    (GTestFunc) test_process);
#if 0
  g_test_add_func (
    TEST_PREFIX "test has custom UI",
    (GTestFunc) test_has_custom_ui);
#endif
  g_test_add_func (
    TEST_PREFIX "test crash handling",
    (GTestFunc) test_crash_handling);

  return g_test_run ();
}
