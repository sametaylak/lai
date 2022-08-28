#include "renderer/renderer_backend.h"
#include "renderer/vulkan/vulkan_backend.h"

bool renderer_backend_create(renderer_backend_type type,
                             renderer_backend *out_renderer_backend) {
  // out_renderer_backend->plat_state = plat_state;

  if (type == RENDERER_BACKEND_TYPE_VULKAN) {
    out_renderer_backend->initialize = vulkan_renderer_backend_initialize;
    out_renderer_backend->shutdown = vulkan_renderer_backend_shutdown;
    out_renderer_backend->resized = vulkan_renderer_backend_on_resized;
    out_renderer_backend->begin_frame = vulkan_renderer_backend_begin_frame;
    out_renderer_backend->end_frame = vulkan_renderer_backend_end_frame;
    return true;
  }

  return false;
}

void renderer_backend_destroy(renderer_backend *renderer_backend) {
  renderer_backend->initialize = nullptr;
  renderer_backend->shutdown = nullptr;
  renderer_backend->resized = nullptr;
  renderer_backend->begin_frame = nullptr;
  renderer_backend->end_frame = nullptr;
}
