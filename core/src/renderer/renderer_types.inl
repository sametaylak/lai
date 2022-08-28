#pragma once

#include "defines.h"

enum renderer_backend_type {
  RENDERER_BACKEND_TYPE_VULKAN,
  RENDERER_BACKEND_TYPE_OPENGL,
  RENDERER_BACKEND_TYPE_DIRECTX
};

struct renderer_backend {
  u64 frame_number;

  bool (*initialize)(struct renderer_backend *backend,
                     const char *application_name);
  void (*shutdown)(struct renderer_backend *backend);
  void (*resized)(struct renderer_backend *backend, u16 width, u16 height);
  bool (*begin_frame)(struct renderer_backend *backend, f32 delta_time);
  bool (*end_frame)(struct renderer_backend *backend, f32 delta_time);
};

struct render_packet {
  f32 delta_time;
};
