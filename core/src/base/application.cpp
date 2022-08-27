#include "base/application.h"
#include "base/clock.h"
#include "base/event.h"
#include "base/input.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "game_types.h"
#include "platform/platform.h"

#include "renderer/renderer_frontend.h"

#include <cstdlib>

struct application_state {
  game *game_inst;
  bool is_running;
  bool is_suspended;
  platform_state platform;
  i16 width;
  i16 height;
  clock clock;
  f64 last_time;
};

static bool initialized = false;
static application_state app_state;

bool application_on_event(u16 code, void *sender, void *listener,
                          event_context context);
bool application_on_key(u16 code, void *sender, void *listener,
                        event_context context);

bool application_create(game *game_inst) {
  if (initialized) {
    LAI_LOG_ERROR("application_create called more than once");
    return false;
  }

  app_state.game_inst = game_inst;

  // Initialize subsystem
  initialize_logging();
  input_initialize();

  app_state.is_running = true;
  app_state.is_suspended = false;

  if (!event_initialize()) {
    LAI_LOG_FATAL("Event system failed to initialize!");
    return false;
  }

  event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

  if (!platform_startup(&app_state.platform, game_inst->app_config.name,
                        game_inst->app_config.start_pos_x,
                        game_inst->app_config.start_pos_y,
                        game_inst->app_config.start_width,
                        game_inst->app_config.start_height)) {
    return false;
  }

  if (!renderer_initialize(game_inst->app_config.name, &app_state.platform)) {
    LAI_LOG_FATAL("Failed to initialize renderer. Shutting down!");
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
  clock_start(&app_state.clock);
  clock_update(&app_state.clock);
  app_state.last_time = app_state.clock.elapsed;

  f64 running_time = 0;
  f64 target_fps = 1.0f / 60;
  u8 frame_count = 0;

  LAI_LOG_INFO(get_memory_usage());

  while (app_state.is_running) {
    clock_update(&app_state.clock);
    f64 current_time = app_state.clock.elapsed;
    f64 delta = (current_time - app_state.last_time);
    f64 frame_start_time = platform_get_absolute_time();

    if (!platform_pump_messages(&app_state.platform)) {
      app_state.is_running = false;
    }

    if (!app_state.is_suspended) {
      if (!app_state.game_inst->update(app_state.game_inst, (f32)delta)) {
        LAI_LOG_FATAL("Game update failed, shutting down!");
        app_state.is_running = true;
        break;
      }

      if (!app_state.game_inst->render(app_state.game_inst, (f32)delta)) {
        LAI_LOG_FATAL("Game render failed, shutting down!");
        app_state.is_running = true;
        break;
      }

      render_packet packet;
      packet.delta_time = delta;
      renderer_draw_frame(&packet);

      f64 frame_end_time = platform_get_absolute_time();
      f64 frame_elapsed_time = frame_end_time - frame_start_time;
      running_time += frame_elapsed_time;
      f64 remaining_seconds = target_fps - frame_elapsed_time;

      if (remaining_seconds > 0) {
        u64 remaining_milliseconds = remaining_seconds * 1000;
        bool limit_frames = false;
        if (remaining_milliseconds > 0 && limit_frames) {
          platform_sleep(remaining_milliseconds - 1);
        }

        frame_count++;
      }

      input_update(delta);

      app_state.last_time = current_time;
    }
  }

  app_state.is_running = false;

  event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

  event_shutdown();
  input_shutdown();
  renderer_shutdown();

  platform_shutdown(&app_state.platform);

  return true;
}

void application_get_framebuffer_size(u32 *width, u32 *height) {
  *width = app_state.width;
  *height = app_state.height;
}

bool application_on_event(u16 code, void *sender, void *listener,
                          event_context context) {
  switch (code) {
  case EVENT_CODE_APPLICATION_QUIT: {
    LAI_LOG_INFO("EVENT_CODE_APPLICATION_QUIT received, shutting down");
    app_state.is_running = false;
    return true;
  }
  }
  return false;
}

bool application_on_key(u16 code, void *sender, void *listener,
                        event_context context) {
  if (code == EVENT_CODE_KEY_PRESSED) {
    u16 key_code = context.data.u16[0];
    if (key_code == KEY_ESCAPE) {
      event_context data;
      event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
      return true;
    } else {
      LAI_LOG_DEBUG("'%c' key pressed", key_code);
    }
  } else if (code == EVENT_CODE_KEY_RELEASED) {
    u16 key_code = context.data.u16[0];
    LAI_LOG_DEBUG("'%c' key released", key_code);
  }
  return false;
}
