#include "base/application.h"
#include "base/clock.h"
#include "base/event.h"
#include "base/input.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "game_types.h"
#include "memory/linear_allocator.h"
#include "platform/platform.h"

#include "renderer/renderer_frontend.h"

#include <cstdlib>

struct application_state {
  game *game_inst;
  bool is_running;
  bool is_suspended;
  i16 width;
  i16 height;
  clock clock;
  f64 last_time;
  linear_allocator systems_allocator;

  u64 memory_system_memory_requirement;
  void *memory_system_state;

  u64 log_system_memory_requirement;
  void *log_system_state;

  u64 input_system_memory_requirement;
  void *input_system_state;

  u64 event_system_memory_requirement;
  void *event_system_state;

  u64 platform_system_memory_requirement;
  void *platform_system_state;

  u64 renderer_system_memory_requirement;
  void *renderer_system_state;
};

static application_state *app_state;

bool application_on_event(u16 code, void *sender, void *listener,
                          event_context context);
bool application_on_key(u16 code, void *sender, void *listener,
                        event_context context);
bool application_on_resized(u16 code, void *sender, void *listener,
                            event_context context);

bool application_create(game *game_inst) {
  if (game_inst->application_state) {
    LAI_LOG_ERROR("application_create called more than once");
    return false;
  }

  game_inst->application_state =
      lai_allocate(sizeof(application_state), MEMORY_TAG_APPLICATION);
  app_state = (application_state *)game_inst->application_state;
  app_state->width = game_inst->app_config.start_width;
  app_state->height = game_inst->app_config.start_height;
  app_state->game_inst = game_inst;
  app_state->is_running = false;
  app_state->is_suspended = false;

  u64 systems_allocator_total_size = 64 * 1024 * 1024; // 64mb
  linear_allocator_create(systems_allocator_total_size, nullptr,
                          &app_state->systems_allocator);

  // Initialize Memory State
  initialize_memory(&app_state->memory_system_memory_requirement, nullptr);
  app_state->memory_system_state =
      linear_allocator_allocate(&app_state->systems_allocator,
                                app_state->memory_system_memory_requirement);
  if (!initialize_memory(&app_state->memory_system_memory_requirement,
                         app_state->memory_system_state)) {
    LAI_LOG_FATAL("Memory system failed to initialize!");
    return false;
  }

  // Initialize Log State
  initialize_logging(&app_state->log_system_memory_requirement, nullptr);
  app_state->log_system_state = linear_allocator_allocate(
      &app_state->systems_allocator, app_state->log_system_memory_requirement);
  if (!initialize_logging(&app_state->log_system_memory_requirement,
                          app_state->log_system_state)) {
    LAI_LOG_FATAL("Log system failed to initialize!");
    return false;
  }

  // Initialize Input State
  initialize_input(&app_state->input_system_memory_requirement, nullptr);
  app_state->input_system_state =
      linear_allocator_allocate(&app_state->systems_allocator,
                                app_state->input_system_memory_requirement);
  if (!initialize_input(&app_state->input_system_memory_requirement,
                        app_state->input_system_state)) {
    LAI_LOG_FATAL("Input system failed to initialize!");
    return false;
  }

  // Initialize Event State
  event_initialize(&app_state->event_system_memory_requirement, nullptr);
  app_state->event_system_state =
      linear_allocator_allocate(&app_state->systems_allocator,
                                app_state->event_system_memory_requirement);
  if (!event_initialize(&app_state->event_system_memory_requirement,
                        app_state->event_system_state)) {
    LAI_LOG_FATAL("Event system failed to initialize!");
    return false;
  }

  event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_register(EVENT_CODE_RESIZED, 0, application_on_resized);
  event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

  // Initialize Platform State
  platform_startup(&app_state->platform_system_memory_requirement, nullptr,
                   nullptr, 0, 0, 0, 0);
  app_state->platform_system_state =
      linear_allocator_allocate(&app_state->systems_allocator,
                                app_state->platform_system_memory_requirement);
  if (!platform_startup(
          &app_state->platform_system_memory_requirement,
          app_state->platform_system_state, game_inst->app_config.name,
          game_inst->app_config.start_pos_x, game_inst->app_config.start_pos_y,
          game_inst->app_config.start_width,
          game_inst->app_config.start_height)) {
    LAI_LOG_FATAL("Platform system failed to initialize!");
    return false;
  }

  // Initialize Renderer State
  renderer_initialize(&app_state->renderer_system_memory_requirement, nullptr,
                      nullptr);
  app_state->renderer_system_state =
      linear_allocator_allocate(&app_state->systems_allocator,
                                app_state->renderer_system_memory_requirement);
  if (!renderer_initialize(&app_state->renderer_system_memory_requirement,
                           app_state->renderer_system_state,
                           game_inst->app_config.name)) {
    LAI_LOG_FATAL("Renderer system failed to initialize!");
    return false;
  }

  if (!app_state->game_inst->initialize(app_state->game_inst)) {
    LAI_LOG_FATAL("Game failed to initialize!");
    return false;
  }

  return true;
}

bool application_run() {
  app_state->is_running = true;

  clock_start(&app_state->clock);
  clock_update(&app_state->clock);
  app_state->last_time = app_state->clock.elapsed;

  f64 running_time = 0;
  f64 target_fps = 1.0f / 60;
  u8 frame_count = 0;

  LAI_LOG_INFO(get_memory_usage());

  while (app_state->is_running) {
    clock_update(&app_state->clock);
    f64 current_time = app_state->clock.elapsed;
    f64 delta = (current_time - app_state->last_time);
    f64 frame_start_time = platform_get_absolute_time();

    if (!platform_pump_messages(app_state->platform_system_state)) {
      app_state->is_running = false;
    }

    if (!app_state->is_suspended) {
      if (!app_state->game_inst->update(app_state->game_inst, (f32)delta)) {
        LAI_LOG_FATAL("Game update failed, shutting down!");
        app_state->is_running = true;
        break;
      }

      if (!app_state->game_inst->render(app_state->game_inst, (f32)delta)) {
        LAI_LOG_FATAL("Game render failed, shutting down!");
        app_state->is_running = true;
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

      app_state->last_time = current_time;
    }
  }

  app_state->is_running = false;

  event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_unregister(EVENT_CODE_RESIZED, 0, application_on_resized);
  event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

  input_shutdown(app_state->input_system_state);
  renderer_shutdown(app_state->renderer_system_state);
  platform_shutdown(app_state->platform_system_state);
  shutdown_memory(app_state->memory_system_state);
  event_shutdown(app_state->event_system_state);
  shutdown_logging(app_state->log_system_state);

  return true;
}

void application_get_framebuffer_size(u32 *width, u32 *height) {
  *width = app_state->width;
  *height = app_state->height;
}

bool application_on_event(u16 code, void *sender, void *listener,
                          event_context context) {
  switch (code) {
  case EVENT_CODE_APPLICATION_QUIT: {
    LAI_LOG_INFO("EVENT_CODE_APPLICATION_QUIT received, shutting down");
    app_state->is_running = false;
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

bool application_on_resized(u16 code, void *sender, void *listener,
                            event_context context) {
  if (code == EVENT_CODE_RESIZED) {
    u16 width = context.data.u16[0];
    u16 height = context.data.u16[1];

    if (width != app_state->width || height != app_state->height) {
      app_state->width = width;
      app_state->height = height;

      LAI_LOG_DEBUG("Window resize, %i, %i", width, height);

      if (width == 0 || height == 0) {
        LAI_LOG_INFO("Window minimized, suspending app");
        app_state->is_suspended = true;
        return true;
      } else {
        if (app_state->is_suspended) {
          LAI_LOG_INFO("Window restored, resuming app");
          app_state->is_suspended = false;
        }
        app_state->game_inst->on_resize(app_state->game_inst, width, height);
        renderer_on_resized(width, height);
      }
    }
  }
  return false;
}