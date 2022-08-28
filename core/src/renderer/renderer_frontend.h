#pragma once

#include "renderer_types.inl"

struct static_mesh_data;

bool renderer_initialize(u64 *memory_requirement, void *state,
                         const char *application_name);
void renderer_shutdown(void *state);
void renderer_on_resized(u16 width, u16 height);
bool renderer_draw_frame(render_packet *packet);
