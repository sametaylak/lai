#include "game.h"

#include <entry_point.h>
#include <platform/platform.h>

bool create_game(game *out_game) {
  out_game->app_config.start_pos_x = 0;
  out_game->app_config.start_pos_y = 0;
  out_game->app_config.start_width = 1600;
  out_game->app_config.start_height = 900;
  out_game->app_config.name = "LAI";

  out_game->update = game_update;
  out_game->render = game_render;
  out_game->initialize = game_initialize;
  out_game->on_resize = game_on_resize;

  out_game->state = lai_allocate(sizeof(game_state), MEMORY_TAG_GAME);

  return true;
}