#pragma once

#include "defines.h"

bool platform_startup(u64 *memory_requirement, void *state, const char *name,
                      i32 x, i32 y, i32 width, i32 height);
bool platform_pump_messages(void *state);
void platform_shutdown(void *state);

void *platform_allocate(u64 size, bool aligned);
void platform_free(void *block, bool aligned);
void *platform_copy_memory(void *destination, const void *source, u64 size);
void *platform_zero_memory(void *block, u64 size);
void *platform_set_memory(void *destination, i32 value, u64 size);

void platform_console_write(const char *message, u8 color);
void platform_console_write_error(const char *message, u8 color);

f64 platform_get_absolute_time();

void platform_sleep(u64 milliseconds);
