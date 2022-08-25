#include "base/log.h"
#include "base/asserts.h"
#include "platform/platform.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line) {
  log_output(LOG_LEVEL_FATAL,
             "Assertion failure: %s, message: %s, in file: %s, line: %d\n",
             expression, message, file, line);
}

bool initialize_logging() {
  // TODO: create .log file
  return true;
}

void shutdown_logging() {
  // TODO: clean queued entries
}

void log_output(log_level level, const char *message, ...) {
  const char *level_strings[6] = {"[FATAL]", "[ERROR]", "[WARN]",
                                  "[INFO]",  "[DEBUG]", "[TRACE]"};
  bool is_error = level < LOG_LEVEL_WARN;

  const i32 message_length = 32000;
  char out_message[message_length];
  memset(out_message, 0, sizeof(out_message));

  __builtin_va_list arg_ptr;
  va_start(arg_ptr, message);
  vsnprintf(out_message, message_length, message, arg_ptr);
  va_end(arg_ptr);

  char out_message2[message_length];
  sprintf(out_message2, "%s %s\n", level_strings[level], out_message);

  if (is_error) {
    platform_console_write_error(out_message, level);
  } else {
    platform_console_write(out_message, level);
  }
}