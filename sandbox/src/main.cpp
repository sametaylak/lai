#include <base/asserts.h>
#include <base/log.h>

int main() {
  LAI_LOG_FATAL("A test message: %f", 3.14f);
  LAI_LOG_ERROR("A test message: %f", 3.14f);
  LAI_LOG_WARN("A test message: %f", 3.14f);
  LAI_LOG_INFO("A test message: %f", 3.14f);
  LAI_LOG_DEBUG("A test message: %f", 3.14f);
  LAI_LOG_TRACE("A test message: %f", 3.14f);

  // LAI_ASSERT(1 == 0);
  // LAI_ASSERT_MESSAGE(1 == 0, "A test message");

  return 0;
}