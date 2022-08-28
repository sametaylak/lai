#include "memory/linear_allocator_tests.h"
#include "expect.h"
#include "test_manager.h"

#include <defines.h>
#include <memory/linear_allocator.h>

u8 linear_allocator_should_create_and_destroy() {
  linear_allocator alloc;
  linear_allocator_create(sizeof(u64), nullptr, &alloc);

  expect_should_not_be(nullptr, alloc.memory);
  expect_should_be(sizeof(u64), alloc.total_size);
  expect_should_be(0, alloc.allocated);

  linear_allocator_destroy(&alloc);
  expect_should_be(0, alloc.total_size);
  expect_should_be(0, alloc.allocated);
  expect_should_be(nullptr, alloc.memory);

  return true;
}

u8 linear_allocator_should_single_allocation_all_space() {
  linear_allocator alloc;
  linear_allocator_create(sizeof(u64), nullptr, &alloc);

  void *block = linear_allocator_allocate(&alloc, sizeof(u64));

  expect_should_not_be(nullptr, block);
  expect_should_be(sizeof(u64), alloc.allocated);

  linear_allocator_destroy(&alloc);

  return true;
}

u8 linear_allocator_should_multi_allocation_all_space() {
  u64 max_allocs = 1024;
  linear_allocator alloc;
  linear_allocator_create(sizeof(u64) * max_allocs, nullptr, &alloc);

  void *block;
  for (u64 i = 0; i < max_allocs; ++i) {
    block = linear_allocator_allocate(&alloc, sizeof(u64));

    expect_should_not_be(nullptr, block);
    expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
  }

  linear_allocator_destroy(&alloc);

  return true;
}

u8 linear_allocator_should_multi_allocation_over_allocate() {
  u64 max_allocs = 3;
  linear_allocator alloc;
  linear_allocator_create(sizeof(u64) * max_allocs, nullptr, &alloc);

  void *block;
  for (u64 i = 0; i < max_allocs; ++i) {
    block = linear_allocator_allocate(&alloc, sizeof(u64));

    expect_should_not_be(nullptr, block);
    expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
  }

  LAI_LOG_DEBUG("Note: the following error is caused by this test!");

  block = linear_allocator_allocate(&alloc, sizeof(u64));
  expect_should_be(nullptr, block);
  expect_should_be(sizeof(u64) * max_allocs, alloc.allocated);

  linear_allocator_destroy(&alloc);

  return true;
}

u8 linear_allocator_should_multi_allocation_all_space_then_free() {
  u64 max_allocs = 1024;
  linear_allocator alloc;
  linear_allocator_create(sizeof(u64) * max_allocs, nullptr, &alloc);

  void *block;
  for (u64 i = 0; i < max_allocs; ++i) {
    block = linear_allocator_allocate(&alloc, sizeof(u64));

    expect_should_not_be(nullptr, block);
    expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
  }

  linear_allocator_free_all(&alloc);
  expect_should_be(0, alloc.allocated);

  linear_allocator_destroy(&alloc);

  return true;
}

void linear_allocator_register_tests() {
  test_manager_register_test(linear_allocator_should_create_and_destroy,
                             "linear_allocator_should_create_and_destroy");
  test_manager_register_test(
      linear_allocator_should_single_allocation_all_space,
      "linear_allocator_should_single_allocation_all_space");
  test_manager_register_test(
      linear_allocator_should_multi_allocation_all_space,
      "linear_allocator_should_multi_allocation_all_space");
  test_manager_register_test(
      linear_allocator_should_multi_allocation_over_allocate,
      "linear_allocator_should_multi_allocation_over_allocate");
  test_manager_register_test(
      linear_allocator_should_multi_allocation_all_space_then_free,
      "linear_allocator_should_multi_allocation_all_space_then_free");
}