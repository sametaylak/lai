#pragma once

#include "defines.h"

#define LAI_ASSERTIONS_ENABLED

#ifdef LAI_ASSERTIONS_ENABLED

#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugBreak()
#else
#define debugBreak() __builtin_trap()
#endif

void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line);

#define LAI_ASSERT(expression)                                                 \
  {                                                                            \
    if (expression) {                                                          \
    } else {                                                                   \
      report_assertion_failure(#expression, "", __FILE__, __LINE__);           \
      debugBreak();                                                            \
    }                                                                          \
  }

#define LAI_ASSERT_MESSAGE(expression, message)                                \
  {                                                                            \
    if (expression) {                                                          \
    } else {                                                                   \
      report_assertion_failure(#expression, message, __FILE__, __LINE__);      \
      debugBreak();                                                            \
    }                                                                          \
  }

#ifdef LAI_DEBUG
#define LAI_ASSERT_DEBUG(expression)                                           \
  {                                                                            \
    if (expression) {                                                          \
    } else {                                                                   \
      report_assertion_failure(#expression, "", __FILE__, __LINE__);           \
      debugBreak();                                                            \
    }                                                                          \
  }
#else
#define LAI_ASSERT_DEBUG(expression) // do nothing
#endif

#else

#define LAI_ASSERT(expression)                  // do nothing
#define LAI_ASSERT_MESSAGE(expression, message) // do nothing
#define LAI_ASSERT_DEBUG(expression)            // do nothing

#endif