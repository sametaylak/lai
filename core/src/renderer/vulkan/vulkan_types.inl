#pragma once

#include "base/asserts.h"
#include "defines.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(expression)                                                   \
  { LAI_ASSERT(expression == VK_SUCCESS); }

struct vulkan_swapchain_support_info {
  VkSurfaceCapabilitiesKHR capabilities;
  u32 format_count;
  VkSurfaceFormatKHR *formats;
  u32 present_mode_count;
  VkPresentModeKHR *present_modes;
};

struct vulkan_device {
  VkPhysicalDevice physical_device;
  VkDevice logical_device;
  vulkan_swapchain_support_info swapchain_support;

  i32 graphics_queue_index;
  i32 present_queue_index;
  i32 transfer_queue_index;

  VkQueue graphics_queue;
  VkQueue present_queue;
  VkQueue transfer_queue;

  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  VkPhysicalDeviceMemoryProperties memory;
};

struct vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;
  VkSurfaceKHR surface;

#ifdef LAI_DEBUG
  VkDebugUtilsMessengerEXT debug_messenger;
#endif

  vulkan_device device;
};