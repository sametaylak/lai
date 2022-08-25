#pragma once

#include <defines.h>
#include <game_types.h>

struct game_state {
  f32 delta_time;
};

bool game_initialize(game *game_inst);
bool game_update(game *game_inst, f32 delta_time);
bool game_render(game *game_inst, f32 delta_time);
void game_on_resize(game *game_inst, u32 width, u32 height);