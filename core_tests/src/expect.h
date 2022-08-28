#pragma once

#include <base/log.h>
#include <math/lai_math.h>

#define expect_should_be(expected, actual)                                     \
  if (actual != expected) {                                                    \
    LAI_LOG_ERROR("--> Expected: %lld, but got: %lld. File: %s:%d", expected,  \
                  actual, __FILE__, __LINE__);                                 \
    return false;                                                              \
  }

#define expect_should_not_be(expected, actual)                                 \
  if (actual == expected) {                                                    \
    LAI_LOG_ERROR("--> Expected: %d != %d, but they are equal. File: %s:%d",   \
                  expected, actual, __FILE__, __LINE__);                       \
    return false;                                                              \
  }

#define expect_float_to_be(expected, actual)                                   \
  if (lai_abs(expected - actual) > 0.001f) {                                   \
    LAI_LOG_ERROR("--> Expected: %f, but got: %f. File: %s:%d", expected,      \
                  actual, __FILE__, __LINE__);                                 \
    return false;                                                              \
  }

#define expect_to_be_true(actual)                                              \
  if (actual != true) {                                                        \
    LAI_LOG_ERROR("--> Expected: true, but got: false. File: %s:%d", expected, \
                  actual, __FILE__, __LINE__);                                 \
    return false;                                                              \
  }

#define expect_to_be_false(actual)                                             \
  if (actual != false) {                                                       \
    LAI_LOG_ERROR("--> Expected: false, but got: true. File: %s:%d", expected, \
                  actual, __FILE__, __LINE__);                                 \
    return false;                                                              \
  }