#include <log.h>

int main() {
  LAI_LOG_FATAL("A test message: %f", 3.14f);
  LAI_LOG_ERROR("A test message: %f", 3.14f);
  LAI_LOG_WARN("A test message: %f", 3.14f);
  LAI_LOG_INFO("A test message: %f", 3.14f);
  LAI_LOG_DEBUG("A test message: %f", 3.14f);
  LAI_LOG_TRACE("A test message: %f", 3.14f);

  return 0;
}