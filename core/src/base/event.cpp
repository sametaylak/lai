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
};

static bool initialized = false;
static event_system_state state;

bool event_initialize() {
  if (initialized) {
    LAI_LOG_ERROR("event_initialize called more than once");
    return false;
  }

  lai_zero_memory(&state, sizeof(state));
  initialized = true;

  LAI_LOG_INFO("Event system initialized!");
  return true;
}

void event_shutdown() {
  for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
    if (state.registered[i].events != nullptr) {
      darray_destroy(state.registered[i].events);
      state.registered[i].events = nullptr;
    }
  }
}

bool event_register(u16 code, void *listener, PFN_on_event on_event) {
  if (initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state.registered[code].events == nullptr) {
    state.registered[code].events =
        (registered_event *)darray_create(registered_event);
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    if (state.registered[code].events[i].listener == listener) {
      return false;
    }
  }

  registered_event event;
  event.listener = listener;
  event.callback = on_event;

  void *event_void = (void *)&event;
  void *events_void = (void *)&state.registered[code].events;
  darray_push(events_void, event_void);

  return true;
}

bool event_unregister(u16 code, void *listener, PFN_on_event on_event) {
  if (initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state.registered[code].events == nullptr) {
    return false;
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    registered_event e = state.registered[code].events[i];
    if (e.listener == listener && e.callback == on_event) {
      // Found one
      registered_event popped_event;
      darray_pop_at(state.registered[code].events, i, &popped_event);
      return true;
    }
  }

  return false;
}

bool event_fire(u16 code, void *sender, event_context context) {
  if (initialized) {
    LAI_LOG_WARN("Waiting for event system initialization");
    return false;
  }

  if (state.registered[code].events == nullptr) {
    return false;
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    registered_event e = state.registered[code].events[i];
    if (e.callback(code, sender, e.listener, context)) {
      return true;
    }
  }

  return false;
}