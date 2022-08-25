#pragma once

#include "base/application.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "game_types.h"

extern bool create_game(game *out_game);

int main() {
  initialize_memory();

  game game_inst;
  if (!create_game(&game_inst)) {
    LAI_LOG_FATAL("Could not create game!");
    return -1;
  }

  if (!game_inst.render || !game_inst.update || !game_inst.initialize ||
      !game_inst.on_resize) {
    LAI_LOG_FATAL("The game's function pointers must be assigned!");
    return -2;
  }

  if (!application_create(&game_inst)) {
    LAI_LOG_INFO("Application failed to create!");
    return 1;
  }

  if (!application_run()) {
    LAI_LOG_INFO("Application did not shutdown gracefully!");
    return 2;
  }

  shutdown_memory();

  return 0;
}