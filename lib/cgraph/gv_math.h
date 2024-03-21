/// \file
/// \brief Arithmetic helper functions
/// \ingroup cgraph_utils

#pragma once

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

/** is a value approximately 0?
 *
 * This can be used for doing a floating point `== 0`, while accounting for
 * floating point inaccuracies. If you need to check against _exactly_ the value
 * 0.0, this function is not what you want.
 *
 * \param v Value to check
 * \param True if the value is close enough to 0 to be considered 0
 */
static inline bool is_zero(double v) { return fabs(v) < DBL_EPSILON; }

/** scale up or down a non-negative integer, clamping to \p [0, INT_MAX]
 *
 * \param original Value to scale
 * \param scale Scale factor to apply
 * \return Clamped result
 */
static inline int scale_clamp(int original, double scale) {
  assert(original >= 0);

  if (scale < 0) {
    return 0;
  }

  if (scale > 1 && original > INT_MAX / scale) {
    return INT_MAX;
  }

  return (int)(original * scale);
}
