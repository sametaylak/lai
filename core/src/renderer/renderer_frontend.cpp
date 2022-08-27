#include "renderer/renderer_frontend.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "renderer/renderer_backend.h"

static renderer_backend *backend;

bool renderer_initialize(const char *application_name,
                         struct platform_state *plat_state) {
  backend = (renderer_backend *)lai_allocate(sizeof(renderer_backend),
                                             MEMORY_TAG_RENDERER);

  renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend);
  backend->frame_number = 0;

  if (!backend->initialize(backend, application_name, plat_state)) {
    LAI_LOG_FATAL("Renderer backend failed, shutting down!");
    return false;
  }

  return true;
}

void renderer_shutdown() {
  backend->shutdown(backend);
  lai_free(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

bool renderer_begin_frame(f32 delta_time) {
  return backend->begin_frame(backend, delta_time);
}

bool renderer_end_frame(f32 delta_time) {
  bool result = backend->end_frame(backend, delta_time);
  backend->frame_number++;
  return result;
}

bool renderer_draw_frame(render_packet *packet) {
  if (renderer_begin_frame(packet->delta_time)) {
    bool result = renderer_end_frame(packet->delta_time);
    if (!result) {
      LAI_LOG_FATAL("Renderer end frame failed, shutting down!");
      return false;
    }
  }

  return true;
}
