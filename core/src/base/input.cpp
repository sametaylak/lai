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

struct input_system_state {
  keyboard_state keyboard_current;
  keyboard_state keyboard_previous;
  mouse_state mouse_current;
  mouse_state mouse_previous;
  bool initialized;
};

static input_system_state *state_ptr;

bool initialize_input(u64 *memory_requirement, void *state) {
  *memory_requirement = sizeof(input_system_state);
  if (state == nullptr) {
    return false;
  }

  state_ptr = (input_system_state *)state;
  state_ptr->initialized = true;

  LAI_LOG_INFO("Input system initialized!");
  return true;
}

void input_shutdown(void *state) { state_ptr = nullptr; }

void input_update(f64 delta_time) {
  if (!state_ptr->initialized) {
    return;
  }

  lai_copy_memory(&state_ptr->keyboard_previous, &state_ptr->keyboard_current,
                  sizeof(keyboard_state));
  lai_copy_memory(&state_ptr->mouse_previous, &state_ptr->mouse_current,
                  sizeof(mouse_state));
}

bool input_is_key_down(keys key) {
  if (!state_ptr->initialized) {
    return false;
  }
  return state_ptr->keyboard_current.keys[key] == true;
}

bool input_is_key_up(keys key) {
  if (!state_ptr->initialized) {
    return true;
  }
  return state_ptr->keyboard_current.keys[key] == false;
}

bool input_was_key_down(keys key) {
  if (!state_ptr->initialized) {
    return false;
  }
  return state_ptr->keyboard_previous.keys[key] == true;
}

bool input_was_key_up(keys key) {
  if (!state_ptr->initialized) {
    return true;
  }
  return state_ptr->keyboard_previous.keys[key] == false;
}

void input_process_key(keys key, bool pressed) {
  if (state_ptr->keyboard_current.keys[key] != pressed) {
    state_ptr->keyboard_current.keys[key] = pressed;

    event_context context;
    context.data.u16[0] = key;
    event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0,
               context);
  }
}

bool input_is_button_down(buttons button) {
  if (!state_ptr->initialized) {
    return false;
  }
  return state_ptr->mouse_current.buttons[button] == true;
}

bool input_is_button_up(buttons button) {
  if (!state_ptr->initialized) {
    return true;
  }
  return state_ptr->mouse_current.buttons[button] == false;
}

bool input_was_button_down(buttons button) {
  if (!state_ptr->initialized) {
    return false;
  }
  return state_ptr->mouse_previous.buttons[button] == true;
}

bool input_was_button_up(buttons button) {
  if (!state_ptr->initialized) {
    return true;
  }
  return state_ptr->mouse_previous.buttons[button] == false;
}

void input_get_mouse_position(i32 *x, i32 *y) {
  if (!state_ptr->initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = state_ptr->mouse_current.x;
  *y = state_ptr->mouse_current.y;
}

void input_get_previous_mouse_position(i32 *x, i32 *y) {
  if (!state_ptr->initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = state_ptr->mouse_previous.x;
  *y = state_ptr->mouse_previous.y;
}

void input_process_button(buttons button, bool pressed) {
  if (state_ptr->mouse_current.buttons[button] != pressed) {
    state_ptr->mouse_current.buttons[button] = pressed;

    event_context context;
    context.data.u16[0] = button;
    event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,
               0, context);
  }
}

void input_process_mouse_move(i16 x, i16 y) {
  if (state_ptr->mouse_current.x != x || state_ptr->mouse_current.y != y) {
    // LAI_LOG_DEBUG("Mouse pos: %i, %i", x, y);
    state_ptr->mouse_current.x = x;
    state_ptr->mouse_current.y = y;

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
