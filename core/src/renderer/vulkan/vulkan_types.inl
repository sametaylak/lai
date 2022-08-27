#pragma once

#include "defines.h"
#include "base/asserts.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(expression) \
  { \
    LAI_ASSERT(expression == VK_SUCCESS); \
  }

struct vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;

#ifdef LAI_DEBUG
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
};