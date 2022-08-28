#include "memory/linear_allocator_tests.h"
#include "test_manager.h"

#include <base/log.h>

int main() {
  LAI_LOG_DEBUG("Starting tests...");
  test_manager_init();

  // Register tests
  linear_allocator_register_tests();

  test_manager_run_tests();

  return 0;
}