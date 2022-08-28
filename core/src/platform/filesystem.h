#pragma once

#include "defines.h"

enum file_modes { FILE_MODE_READ = 0x1, FILE_MODE_WRITE = 0x2 };

struct file_handle {
  void *handle;
  bool is_valid;
};

bool filesystem_exists(const char *path);

bool filesystem_open(const char *path, file_modes mode, bool binary,
                     file_handle *out_handle);

void filesystem_close(file_handle *handle);

bool filesystem_read_line(file_handle *handle, char **line_buf);

bool filesystem_write_line(file_handle *handle, const char *text);

bool filesystem_read(file_handle *handle, u64 data_size, void *out_data,
                     u64 *out_bytes_read);

bool filesystem_read_all_bytes(file_handle *handle, u8 **out_bytes,
                               u64 *out_bytes_read);

bool filesystem_write(file_handle *handle, u64 data_size, const void *data,
                      u64 *out_bytes_written);