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

#ifndef __UTILS_DSP_H__
#define __UTILS_DSP_H__

#include "zrythm-config.h"

#include <stdbool.h>
#include <stddef.h>

#include "utils/math.h"
#include "zrythm.h"

#include <glib.h>

#ifdef HAVE_LSP_DSP
#include <lsp-plug.in/dsp/dsp.h>
#endif

/**
 * Fill the buffer with the given value.
 */
HOT
NONNULL
void
dsp_fill (
  float * buf,
  float   val,
  size_t  size);

/**
 * Clamp the buffer to min/max.
 */
NONNULL
HOT
static inline void
dsp_limit1 (
  float * buf,
  float   minf,
  float   maxf,
  size_t  size)
{
#ifdef HAVE_LSP_DSP
  if (ZRYTHM_USE_OPTIMIZED_DSP)
    {
      lsp_dsp_limit1 (buf, minf, maxf, size);
    }
  else
    {
#endif
      for (size_t i = 0; i < size; i++)
        {
          buf[i] = CLAMP (buf[i], minf, maxf);
        }
#ifdef HAVE_LSP_DSP
    }
#endif
}

NONNULL
HOT
void
dsp_copy (
  float *       dest,
  const float * src,
  size_t        size);

/**
 * Scale: dst[i] = dst[i] * k.
 */
NONNULL
HOT
void
dsp_mul_k2 (
  float * dest,
  float   k,
  size_t  size);

/**
 * Gets the absolute max of the buffer.
 *
 * @return Whether the peak changed.
 */
HOT
NONNULL
static inline bool
dsp_abs_max (
  float * buf,
  float * cur_peak,
  size_t  size)
{
  float new_peak = *cur_peak;

#ifdef HAVE_LSP_DSP
  if (ZRYTHM_USE_OPTIMIZED_DSP)
    {
      new_peak =
        lsp_dsp_abs_max (buf, size);
    }
  else
    {
#endif
      for (size_t i = 0; i < size; i++)
        {
          float val = fabsf (buf[i]);
          if (val > new_peak)
            {
              new_peak = val;
            }
        }
#ifdef HAVE_LSP_DSP
    }
#endif

  bool changed =
    !math_floats_equal (new_peak, *cur_peak);
  *cur_peak = new_peak;

  return changed;
}

/**
 * Gets the minimum of the buffer.
 */
NONNULL
float
dsp_min (
  float * buf,
  size_t  size);

/**
 * Gets the maximum of the buffer.
 */
NONNULL
float
dsp_max (
  float * buf,
  size_t  size);

/**
 * Calculate dst[i] = dst[i] + src[i].
 */
NONNULL
HOT
void
dsp_add2 (
  float *       dest,
  const float * src,
  size_t        count);

/**
 * Calculate dest[i] = dest[i] * k1 + src[i] * k2.
 */
NONNULL
HOT
static inline void
dsp_mix2 (
  float *       dest,
  const float * src,
  float         k1,
  float         k2,
  size_t        size)
{
#ifdef HAVE_LSP_DSP
  if (ZRYTHM_USE_OPTIMIZED_DSP)
    {
      lsp_dsp_mix2 (dest, src, k1, k2, size);
    }
  else
    {
#endif
      for (size_t i = 0; i < size; i++)
        {
          dest[i] = dest[i] * k1 + src[i] * k2;
        }
#ifdef HAVE_LSP_DSP
    }
#endif
}

/**
 * Calculate
 * dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2.
 */
NONNULL
void
dsp_mix_add2 (
  float *       dest,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  size_t        size);

/**
 * Calculate linear fade in by multiplying from
 * 0 to 1.
 */
NONNULL
void
dsp_linear_fade_in (
  float * dest,
  size_t  size);

/**
 * Calculate linear fade in by multiplying from
 * 1 to 0.
 */
NONNULL
void
dsp_linear_fade_out (
  float * dest,
  size_t  size);

/**
 * Makes the two signals mono.
 *
 * @param equal_power True for equal power, false
 *   for equal amplitude.
 *
 * @note Equal amplitude is more suitable for mono
 * compatibility checking. For reference:
 * equal power sum =
 * (L+R) * 0.7079 (-3dB)
 * equal amplitude sum =
 * (L+R) /2 (-6.02dB)
 */
NONNULL
void
dsp_make_mono (
  float * l,
  float * r,
  size_t  size,
  bool    equal_power);

#endif
