#include "renderer/renderer_frontend.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "renderer/renderer_backend.h"

struct renderer_system_state {
  renderer_backend *backend;
};
static renderer_system_state *state_ptr;

bool renderer_initialize(u64 *memory_requirement, void *state,
                         const char *application_name) {

  *memory_requirement = sizeof(renderer_system_state);
  if (state == nullptr) {
    return false;
  }

  state_ptr = (renderer_system_state *)state;
  state_ptr->backend = (renderer_backend *)lai_allocate(
      sizeof(renderer_backend), MEMORY_TAG_RENDERER);
  renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, state_ptr->backend);
  state_ptr->backend->frame_number = 0;

  if (!state_ptr->backend->initialize(state_ptr->backend, application_name)) {
    LAI_LOG_FATAL("Renderer backend failed, shutting down!");
    return false;
  }

  return true;
}

void renderer_shutdown(void *state) {
  if (state_ptr) {
    state_ptr->backend->shutdown(state_ptr->backend);
    lai_free(state_ptr->backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);

    state_ptr = nullptr;
  }
}

void renderer_on_resized(u16 width, u16 height) {
  if (state_ptr->backend) {
    state_ptr->backend->resized(state_ptr->backend, width, height);
  } else {
    LAI_LOG_WARN("Renderer backend does not exist to accept resize");
  }
}

bool renderer_begin_frame(f32 delta_time) {
  return state_ptr->backend->begin_frame(state_ptr->backend, delta_time);
}

bool renderer_end_frame(f32 delta_time) {
  bool result = state_ptr->backend->end_frame(state_ptr->backend, delta_time);
  state_ptr->backend->frame_number++;
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
