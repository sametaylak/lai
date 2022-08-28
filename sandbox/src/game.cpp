#include "game.h"

#include <base/input.h>
#include <base/lai_memory.h>
#include <base/log.h>

bool game_initialize(game *game_inst) {
  LAI_LOG_DEBUG("Game initialize called!");
  return true;
}

bool game_update(game *game_inst, f32 delta_time) {
  if (input_is_key_up(KEY_M) && input_was_key_down(KEY_M)) {
    LAI_LOG_DEBUG("%s", get_memory_usage());
  }

  return true;
}

bool game_render(game *game_inst, f32 delta_time) { return true; }

void game_on_resize(game *game_inst, u32 width, u32 height) {
  LAI_LOG_DEBUG("Game on resize called!");
}
