#include "game.h"

#include <base/input.h>
#include <base/lai_memory.h>
#include <base/log.h>

bool game_initialize(game *game_inst) {
  LAI_LOG_DEBUG("Game initialize called!");
  return true;
}

bool game_update(game *game_inst, f32 delta_time) {
  static u64 alloc_count = 0;
  u64 prev_alloc_count = alloc_count;
  alloc_count = get_memory_alloc_count();
  if (input_is_key_up(KEY_M) && input_was_key_down(KEY_M)) {
    LAI_LOG_DEBUG("Allocations: %llu (%llu this frame)", alloc_count,
                  alloc_count - prev_alloc_count);
  }

  return true;
}

bool game_render(game *game_inst, f32 delta_time) { return true; }

void game_on_resize(game *game_inst, u32 width, u32 height) {
  LAI_LOG_DEBUG("Game on resize called!");
}
