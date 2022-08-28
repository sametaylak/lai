#pragma once

#include "defines.h"

u64 string_length(const char *str);
char *string_duplicate(const char *str);
bool strings_equal(const char *str0, const char *str1);
i32 string_format(char *dest, const char *format, ...);
i32 string_format_v(char *dest, const char *format, void *va_list);