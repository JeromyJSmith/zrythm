/*
 * Copyright (C) 2018-2021 Alexandros Theodotou <alex at zrythm dot org>
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

#include <math.h>

#include "utils/math.h"
#include "utils/string.h"

#include <gtk/gtk.h>

#include <float.h>

/**
 * Gets the RMS of the given signal as amplitude
 * (0-2).
 */
sample_t
math_calculate_rms_amp (
  sample_t *      buf,
  const nframes_t nframes)
{
  sample_t sum = 0, sample = 0;
  for (unsigned int i = 0; i < nframes;
       i += MATH_RMS_FRAMES)
    {
      sample = buf[i];
      sum += (sample * sample);
    }
  return sqrtf (
    sum
    / ((sample_t) nframes / (sample_t) MATH_RMS_FRAMES));
}

/**
 * Calculate db using RMS method.
 *
 * @param buf Buffer containing the samples.
 * @param nframes Number of samples.
 */
sample_t
math_calculate_rms_db (
  sample_t *      buf,
  const nframes_t nframes)
{
  return math_amp_to_dbfs (
    math_calculate_rms_amp (buf, nframes));
}

/**
 * Asserts that the value is non-nan.
 *
 * Not real-time safe.
 *
 * @return Whether the value is valid (nonnan).
 */
bool
math_assert_nonnann (float x)
{
  char * val = g_strdup_printf ("%f", (double) x);
  if (isnan (x) || string_contains_substr (val, "nan"))
    {
      g_critical ("nan");
      g_free (val);
      return false;
    }
  if (
    !isfinite (x)
    || string_contains_substr (val, "inf"))
    {
      g_critical ("inf");
      g_free (val);
      return false;
    }
  return true;
}
