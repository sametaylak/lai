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

  VkFormat depth_format;
};

struct vulkan_image {
  VkImage handle;
  VkDeviceMemory memory;
  VkImageView view;
  u32 width;
  u32 height;
};

struct vulkan_swapchain {
  VkSurfaceFormatKHR image_format;
  u8 max_frames_in_flight;
  VkSwapchainKHR handle;
  u32 image_count;
  VkImage *images;
  VkImageView *views;

  vulkan_image depth_attachment;
};

struct vulkan_context {
  u32 framebuffer_width;
  u32 framebuffer_height;
  VkInstance instance;
  VkAllocationCallbacks *allocator;
  VkSurfaceKHR surface;

#ifdef LAI_DEBUG
  VkDebugUtilsMessengerEXT debug_messenger;
#endif

  vulkan_device device;

  vulkan_swapchain swapchain;
  u32 image_index;
  u32 current_frame;

  bool recreating_swapchain;

  i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
};