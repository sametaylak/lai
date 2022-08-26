#include "base/lai_string.h"
#include "base/lai_memory.h"

#include <string>

u64 string_length(const char *str) { return strlen(str); }

char *string_duplicate(const char *str) {
  u64 length = string_length(str);
  char *copy = (char *)lai_allocate(length + 1, MEMORY_TAG_STRING);
  lai_copy_memory(copy, str, length + 1);
  return copy;
}