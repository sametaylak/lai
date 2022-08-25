#pragma once

#include "base/application.h"

struct game {
  application_config app_config;
  void *state;
  bool (*initialize)(struct game *game_inst);
  bool (*update)(struct game *game_inst, f32 delta_time);
  bool (*render)(struct game *game_inst, f32 delta_time);
  void (*on_resize)(struct game *game_inst, u32 width, u32 height);
};