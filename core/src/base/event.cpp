#include "base/event.h"
#include "base/lai_memory.h"
#include "base/log.h"
#include "containers/darray.h"

struct registered_event {
  void *listener;
  PFN_on_event callback;
};

struct event_code_entry {
  registered_event *events;
};

#define MAX_MESSAGE_CODES 16384

struct event_system_state {
  event_code_entry registered[MAX_MESSAGE_CODES];
  bool initialized;
};
static event_system_state *state_ptr;

bool event_initialize(u64 *memory_requirement, void *state) {
  *memory_requirement = sizeof(event_system_state);
  if (state == nullptr) {
    return false;
  }

  state_ptr = (event_system_state *)state;
  state_ptr->initialized = true;

  LAI_LOG_INFO("Event system initialized!");
  return true;
}

void event_shutdown(void *state) {
  for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
    if (state_ptr->registered[i].events != nullptr) {
      darray_destroy(state_ptr->registered[i].events);
      state_ptr->registered[i].events = nullptr;
    }
  }

  state_ptr = nullptr;
}

bool event_register(u16 code, void *listener, PFN_on_event on_event) {
  if (!state_ptr->initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state_ptr->registered[code].events == nullptr) {
    state_ptr->registered[code].events = darray_create(registered_event);
  }

  u64 registered_count = darray_length(state_ptr->registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    if (state_ptr->registered[code].events[i].listener == listener) {
      return false;
    }
  }

  registered_event event;
  event.listener = listener;
  event.callback = on_event;

  darray_push(state_ptr->registered[code].events, event);

  return true;
}

bool event_unregister(u16 code, void *listener, PFN_on_event on_event) {
  if (!state_ptr->initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state_ptr->registered[code].events == nullptr) {
    return false;
  }

  u64 registered_count = darray_length(state_ptr->registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    registered_event e = state_ptr->registered[code].events[i];
    if (e.listener == listener && e.callback == on_event) {
      // Found one
      registered_event popped_event;
      darray_pop_at(state_ptr->registered[code].events, i, &popped_event);
      return true;
    }
  }

  return false;
}

bool event_fire(u16 code, void *sender, event_context context) {
  if (!state_ptr->initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state_ptr->registered[code].events == nullptr) {
    return false;
  }

  u64 registered_count = darray_length(state_ptr->registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    registered_event e = state_ptr->registered[code].events[i];
    if (e.callback(code, sender, e.listener, context)) {
      return true;
    }
  }

  return false;
}