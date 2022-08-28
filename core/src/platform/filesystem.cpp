#include "platform/filesystem.h"

#include "base/lai_memory.h"
#include "base/log.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

bool filesystem_exists(const char *path) {
  struct stat buffer;
  return stat(path, &buffer) == 0;
}

bool filesystem_open(const char *path, file_modes mode, bool binary,
                     file_handle *out_handle) {
  out_handle->is_valid = false;
  out_handle->handle = nullptr;

  const char *mode_str;

  if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) != 0) {
    mode_str = binary ? "w+b" : "w+";
  } else if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) == 0) {
    mode_str = binary ? "rb" : "r";
  } else if ((mode & FILE_MODE_READ) == 0 && (mode & FILE_MODE_WRITE) != 0) {
    mode_str = binary ? "wb" : "w";
  } else {
    LAI_LOG_ERROR("Invalid mode passed while trying to open file: %s", path);
    return false;
  }

  FILE *file = fopen(path, mode_str);
  if (!file) {
    LAI_LOG_ERROR("Error opening file: %s", path);
    return false;
  }

  out_handle->handle = file;
  out_handle->is_valid = true;

  return true;
}

void filesystem_close(file_handle *handle) {
  if (handle->handle) {
    fclose((FILE *)handle->handle);
    handle->handle = nullptr;
    handle->is_valid = false;
  }
}

bool filesystem_read_line(file_handle *handle, char **line_buf) {
  if (handle->handle) {
    char buffer[32000];
    if (fgets(buffer, 32000, (FILE *)handle->handle) != 0) {
      u64 length = strlen(buffer);
      *line_buf =
          (char *)lai_allocate((sizeof(char) * length) + 1, MEMORY_TAG_STRING);
      strcpy(*line_buf, buffer);
      return true;
    }
  }
  return false;
}

bool filesystem_write_line(file_handle *handle, const char *text) {
  if (handle->handle) {
    i32 result = fputs(text, (FILE *)handle->handle);
    if (result != EOF) {
      result = putc('\n', (FILE *)handle->handle);
    }

    fflush((FILE *)handle->handle);
    return result != EOF;
  }
  return false;
}

bool filesystem_read(file_handle *handle, u64 data_size, void *out_data,
                     u64 *out_bytes_read) {
  if (handle->handle && out_data) {
    *out_bytes_read = fread(out_data, 1, data_size, (FILE *)handle->handle);
    if (*out_bytes_read != data_size) {
      return false;
    }
    return true;
  }
  return false;
}

bool filesystem_read_all_bytes(file_handle *handle, u8 **out_bytes,
                               u64 *out_bytes_read) {
  if (handle->handle) {
    fseek((FILE *)handle->handle, 0, SEEK_END);
    u64 size = ftell((FILE *)handle->handle);
    rewind((FILE *)handle->handle);

    *out_bytes = (u8 *)lai_allocate(sizeof(u8) * size, MEMORY_TAG_STRING);
    *out_bytes_read = fread(*out_bytes, 1, size, (FILE *)handle->handle);
    if (*out_bytes_read != size) {
      return false;
    }
    return true;
  }
  return false;
}

bool filesystem_write(file_handle *handle, u64 data_size, const void *data,
                      u64 *out_bytes_written) {
  if (handle->handle) {
    *out_bytes_written = fwrite(data, 1, data_size, (FILE *)handle->handle);
    if (*out_bytes_written != data_size) {
      return false;
    }
    fflush((FILE *)handle->handle);
    return true;
  }
  return false;
}