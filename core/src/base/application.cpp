#include "base/application.h"
#include "base/log.h"
#include "game_types.h"
#include "platform/platform.h"

#include <cstdlib>

struct application_state {
  game *game_inst;
  bool is_running;
  bool is_suspended;
  platform_state platform;
  i16 width;
  i16 height;
  f64 last_time;
};

static bool initialized = false;
static application_state app_state;

bool application_create(game *game_inst) {
  if (initialized) {
    LAI_LOG_ERROR("application_create called more than once");
    return false;
  }

  app_state.game_inst = game_inst;

  // Initialize subsystem
  initialize_logging();

  // TODO: remove
  LAI_LOG_FATAL("A test message: %f", 3.14f);
  LAI_LOG_ERROR("A test message: %f", 3.14f);
  LAI_LOG_WARN("A test message: %f", 3.14f);
  LAI_LOG_INFO("A test message: %f", 3.14f);
  LAI_LOG_DEBUG("A test message: %f", 3.14f);
  LAI_LOG_TRACE("A test message: %f", 3.14f);

  app_state.is_running = true;
  app_state.is_suspended = false;

  if (!platform_startup(&app_state.platform, game_inst->app_config.name,
                        game_inst->app_config.start_pos_x,
                        game_inst->app_config.start_pos_y,
                        game_inst->app_config.start_width,
                        game_inst->app_config.start_height)) {
    return false;
  }

  if (!app_state.game_inst->initialize(app_state.game_inst)) {
    LAI_LOG_FATAL("Game failed to initialize!");
    return false;
  }

  app_state.game_inst->on_resize(app_state.game_inst, app_state.width,
                                 app_state.height);

  initialized = true;

  return true;
}

bool application_run() {
  while (app_state.is_running) {
    if (!platform_pump_messages(&app_state.platform)) {
      app_state.is_running = false;
    }

    if (!app_state.is_suspended) {
      if (!app_state.game_inst->update(app_state.game_inst, 0)) {
        LAI_LOG_FATAL("Game update failed, shutting down!");
        app_state.is_running = true;
        break;
      }

      if (!app_state.game_inst->render(app_state.game_inst, 0)) {
        LAI_LOG_FATAL("Game render failed, shutting down!");
        app_state.is_running = true;
        break;
      }
    }
  }

  app_state.is_running = false;

  platform_shutdown(&app_state.platform);

  return true;
}