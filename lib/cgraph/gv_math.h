/// \file
/// \brief Arithmetic helper functions
/// \ingroup cgraph_utils

#pragma once

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

/// comparator for doubles
static inline int fcmp(double a, double b) {
  if (a < b) {
    return -1;
  }
  if (a > b) {
    return 1;
  }
  return 0;
}

/// maximum of two integers
static inline int imax(int a, int b) { return a > b ? a : b; }

/// minimum of two integers
static inline int imin(int a, int b) { return a < b ? a : b; }

/**
 * \brief are two values precisely the same?
 *
 * This function should only be used when you know you want comparison with no
 * tolerance, which is rare. Floating-point arithmetic accumulates imprecision,
 * so equality comparisons should generally include a non-zero tolerance to
 * account for this. In general, this function is only applicable for checking
 * things like “is this variable unchanged since a previous assignment from a
 * literal?”
 *
 * \param a First operand to comparison
 * \param b Second operand to comparison
 * \return True if the values are equal
 */
static inline bool is_exactly_equal(double a, double b) {
  return memcmp(&a, &b, sizeof(a)) == 0;
}

/**
 * \brief is a value precisely 0.0?
 *
 * This function should only be used when you know you want comparison with no
 * tolerance, which is rare. Floating-point arithmetic accumulates imprecision,
 * so equality comparisons should generally include a non-zero tolerance to
 * account for this. Valid `double` representations even include -0.0, for which
 * this function will return false. In general, this function is only applicable
 * for checking things like “is this variable unchanged since a previous
 * assignment from the literal `0`?” or “did this value we parsed from user
 * input originate from the string "0.0"?”
 *
 * \param v Value to check
 * \return True if the value is equal to exactly 0.0
 */
static inline bool is_exactly_zero(double v) { return is_exactly_equal(v, 0); }

/**
 * \brief scale up or down a non-negative integer, clamping to \p [0, INT_MAX]
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
