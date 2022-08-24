#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1

#ifdef LAI_RELEASE
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#else
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
#endif

enum log_level {
  LOG_LEVEL_FATAL = 0,
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_TRACE = 5
};

bool initialize_logging();
void shutdown_logging();

void log_output(log_level level, const char *message, ...);

#define LAI_LOG_FATAL(message, ...)                                            \
  log_output(LOG_LEVEL_FATAL, message, __VA_ARGS__)
#define LAI_LOG_ERROR(message, ...)                                            \
  log_output(LOG_LEVEL_ERROR, message, __VA_ARGS__)
#define LAI_LOG_WARN(message, ...)                                             \
  log_output(LOG_LEVEL_WARN, message, __VA_ARGS__)
#define LAI_LOG_INFO(message, ...)                                             \
  log_output(LOG_LEVEL_INFO, message, __VA_ARGS__)

#if LOG_DEBUG_ENABLED == 1
#define LAI_LOG_DEBUG(message, ...)                                            \
  log_output(LOG_LEVEL_DEBUG, message, __VA_ARGS__)
#else
#define LAI_LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define LAI_LOG_TRACE(message, ...)                                            \
  log_output(LOG_LEVEL_TRACE, message, __VA_ARGS__)
#else
#define LAI_LOG_TRACE(message, ...)
#endif
