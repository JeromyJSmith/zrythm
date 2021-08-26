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

#include "audio/transport.h"
#include "project.h"
#include "utils/flags.h"
#include "zrythm.h"

#include "tests/helpers/plugin_manager.h"
#include "tests/helpers/project.h"

#include <glib.h>

static void
test_load_project_bpm (void)
{
  test_helper_zrythm_init ();

  Position pos;
  position_init (&pos);
  bpm_t bpm_before =
    tempo_track_get_bpm_at_pos (P_TEMPO_TRACK, &pos);
  tempo_track_set_bpm (
    P_TEMPO_TRACK, bpm_before + 20.f, bpm_before,
    false, F_NO_PUBLISH_EVENTS);

  /* save and reload the project */
  test_project_save_and_reload ();

  bpm_t bpm_after =
    tempo_track_get_bpm_at_pos (P_TEMPO_TRACK, &pos);
  g_assert_cmpfloat_with_epsilon (
    bpm_after, bpm_before + 20.f, 0.0001f);

  test_helper_zrythm_cleanup ();
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

#define TEST_PREFIX "/audio/transport/"

  g_test_add_func (
    TEST_PREFIX "test load project bpm",
    (GTestFunc) test_load_project_bpm);

  return g_test_run ();
}
