#include "game.h"

#include <base/log.h>

bool game_initialize(game *game_inst) {
  LAI_LOG_DEBUG("Game initialize called!");
  return true;
}

bool game_update(game *game_inst, f32 delta_time) {
  // LAI_LOG_DEBUG("Game update called!");
  return true;
}

bool game_render(game *game_inst, f32 delta_time) {
  // LAI_LOG_DEBUG("Game render called!");
  return true;
}

void game_on_resize(game *game_inst, u32 width, u32 height) {
  LAI_LOG_DEBUG("Game on resize called!");
}
