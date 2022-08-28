#include "test_manager.h"

#include <base/lai_string.h>
#include <base/log.h>
#include <containers/darray.h>

struct test_entry {
  PFN_test func;
  char *description;
};

static test_entry *tests;

void test_manager_init() { tests = darray_create(test_entry); }

void test_manager_register_test(u8 (*PFN_test)(), char *description) {
  test_entry e;
  e.func = PFN_test;
  e.description = description;

  darray_push(tests, e);
}

void test_manager_run_tests() {
  u32 passed = 0;
  u32 failed = 0;
  u32 skipped = 0;

  u32 count = darray_length(tests);

  for (u32 i = 0; i < count; ++i) {
    u8 result = tests[i].func();

    if (result == true) {
      ++passed;
    } else if (result == BYPASS) {
      LAI_LOG_WARN("[SKIPPED]: %s", tests[i].description);
      ++skipped;
    } else {
      LAI_LOG_ERROR("[FAILED]: %s", tests[i].description);
      ++failed;
    }
    char status[20];
    string_format(status, failed ? "*** %d FAILED ***" : "SUCCESS", failed);
    LAI_LOG_INFO("Executed %d of %d", i + 1, count, skipped, status);
  }

  LAI_LOG_INFO("Results: %d passed, %d failed, %d skipped.", passed, failed,
               skipped);
}