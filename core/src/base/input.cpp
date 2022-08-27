#include "base/input.h"
#include "base/event.h"
#include "base/lai_memory.h"
#include "base/log.h"

struct keyboard_state {
  bool keys[256];
};

struct mouse_state {
  i16 x;
  i16 y;
  u8 buttons[BUTTON_MAX_BUTTONS];
};

struct input_state {
  keyboard_state keyboard_current;
  keyboard_state keyboard_previous;
  mouse_state mouse_current;
  mouse_state mouse_previous;
};

static bool initialized = false;
static input_state state;

void input_initialize() {
  lai_zero_memory(&state, sizeof(input_state));
  initialized = true;

  LAI_LOG_INFO("Input system initialized!");
}

void input_shutdown() { initialized = false; }

void input_update(f64 delta_time) {
  if (!initialized) {
    return;
  }

  lai_copy_memory(&state.keyboard_previous, &state.keyboard_current,
                  sizeof(keyboard_state));
  lai_copy_memory(&state.mouse_previous, &state.mouse_current,
                  sizeof(mouse_state));
}

bool input_is_key_down(keys key) {
  if (!initialized) {
    return false;
  }
  return state.keyboard_current.keys[key] == true;
}

bool input_is_key_up(keys key) {
  if (!initialized) {
    return true;
  }
  return state.keyboard_current.keys[key] == false;
}

bool input_was_key_down(keys key) {
  if (!initialized) {
    return false;
  }
  return state.keyboard_previous.keys[key] == true;
}

bool input_was_key_up(keys key) {
  if (!initialized) {
    return true;
  }
  return state.keyboard_previous.keys[key] == false;
}

void input_process_key(keys key, bool pressed) {
  if (state.keyboard_current.keys[key] != pressed) {
    state.keyboard_current.keys[key] = pressed;

    event_context context;
    context.data.u16[0] = key;
    event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0,
               context);
  }
}

bool input_is_button_down(buttons button) {
  if (!initialized) {
    return false;
  }
  return state.mouse_current.buttons[button] == true;
}

bool input_is_button_up(buttons button) {
  if (!initialized) {
    return true;
  }
  return state.mouse_current.buttons[button] == false;
}

bool input_was_button_down(buttons button) {
  if (!initialized) {
    return false;
  }
  return state.mouse_previous.buttons[button] == true;
}

bool input_was_button_up(buttons button) {
  if (!initialized) {
    return true;
  }
  return state.mouse_previous.buttons[button] == false;
}

void input_get_mouse_position(i32 *x, i32 *y) {
  if (!initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = state.mouse_current.x;
  *y = state.mouse_current.y;
}

void input_get_previous_mouse_position(i32 *x, i32 *y) {
  if (!initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = state.mouse_previous.x;
  *y = state.mouse_previous.y;
}

void input_process_button(buttons button, bool pressed) {
  if (state.mouse_current.buttons[button] != pressed) {
    state.mouse_current.buttons[button] = pressed;

    event_context context;
    context.data.u16[0] = button;
    event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,
               0, context);
  }
}

void input_process_mouse_move(i16 x, i16 y) {
  if (state.mouse_current.x != x || state.mouse_current.y != y) {
    // LAI_LOG_DEBUG("Mouse pos: %i, %i", x, y);
    state.mouse_current.x = x;
    state.mouse_current.y = y;

    event_context context;
    context.data.u16[0] = x;
    context.data.u16[1] = y;
    event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
  }
}

void input_process_mouse_wheel(i8 z_delta) {
  event_context context;
  context.data.u8[0] = z_delta;
  event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}
