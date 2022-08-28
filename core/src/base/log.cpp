#include "base/log.h"
#include "base/asserts.h"
#include "base/lai_memory.h"
#include "base/lai_string.h"
#include "platform/filesystem.h"
#include "platform/platform.h"

#include <cstdarg>

void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line) {
  log_output(LOG_LEVEL_FATAL,
             "Assertion failure: %s, message: %s, in file: %s, line: %d\n",
             expression, message, file, line);
}

struct log_system_state {
  file_handle log_file_handle;
};
static log_system_state *state_ptr;

void append_to_log_file(const char *message) {
  if (state_ptr && state_ptr->log_file_handle.is_valid) {
    u64 length = string_length(message);
    u64 written = 0;
    if (!filesystem_write(&state_ptr->log_file_handle, length, message,
                          &written)) {
      platform_console_write_error("ERROR: Could not write to console.log",
                                   LOG_LEVEL_ERROR);
    }
  }
}

bool initialize_logging(u64 *memory_requirement, void *state) {
  *memory_requirement = sizeof(log_system_state);
  if (state == nullptr) {
    return false;
  }

  state_ptr = (log_system_state *)state;

  if (!filesystem_open("console.log", FILE_MODE_WRITE, false,
                       &state_ptr->log_file_handle)) {
    platform_console_write_error("ERROR: Could not open console.log",
                                 LOG_LEVEL_ERROR);
    return false;
  }

  // TODO: create .log file
  return true;
}

void shutdown_logging(void *state) {
  // TODO: clean queued entries
  state_ptr = nullptr;
}

void log_output(log_level level, const char *message, ...) {
  const char *level_strings[6] = {"[FATAL]", "[ERROR]", "[WARN]",
                                  "[INFO]",  "[DEBUG]", "[TRACE]"};
  bool is_error = level < LOG_LEVEL_WARN;

  const i32 message_length = 32000;
  char out_message[message_length];
  lai_zero_memory(out_message, sizeof(out_message));

  __builtin_va_list arg_ptr;
  va_start(arg_ptr, message);
  string_format_v(out_message, message, arg_ptr);
  va_end(arg_ptr);

  string_format(out_message, "%s%s\n", level_strings[level], out_message);

  if (is_error) {
    platform_console_write_error(out_message, level);
  } else {
    platform_console_write(out_message, level);
  }

  append_to_log_file(out_message);
}