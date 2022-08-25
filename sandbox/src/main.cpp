#include <base/log.h>
#include <platform/platform.h>

int main() {
  LAI_LOG_FATAL("A test message: %f", 3.14f);
  LAI_LOG_ERROR("A test message: %f", 3.14f);
  LAI_LOG_WARN("A test message: %f", 3.14f);
  LAI_LOG_INFO("A test message: %f", 3.14f);
  LAI_LOG_DEBUG("A test message: %f", 3.14f);
  LAI_LOG_TRACE("A test message: %f", 3.14f);

  platform_state state;
  if (platform_startup(&state, "LAI", 0, 0, 800, 600)) {
    while (true) {
      platform_pump_messages(&state);
    }
  }
  platform_shutdown(&state);

  return 0;
}