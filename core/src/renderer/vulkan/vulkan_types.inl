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

  VkCommandPool graphics_command_pool;

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

enum vulkan_renderpass_state {
  READY,
  RECORDING,
  IN_RENDERPASS,
  RECORDING_ENDED,
  SUBMITTED,
  NOT_ALLOCATED
};

struct vulkan_renderpass {
  VkRenderPass handle;
  f32 x, y, w, h;
  f32 r, g, b, a;

  f32 depth;
  u32 stencil;

  vulkan_renderpass_state state;
};

struct vulkan_framebuffer {
  VkFramebuffer handle;
  u32 attachment_count;
  VkImageView *attachments;
  vulkan_renderpass *renderpass;
};

struct vulkan_swapchain {
  VkSurfaceFormatKHR image_format;
  u8 max_frames_in_flight;
  VkSwapchainKHR handle;
  u32 image_count;
  VkImage *images;
  VkImageView *views;

  vulkan_image depth_attachment;

  vulkan_framebuffer *framebuffers;
};

enum vulkan_command_buffer_state {
  COMMAND_BUFFER_STATE_READY,
  COMMAND_BUFFER_STATE_RECORDING,
  COMMAND_BUFFER_STATE_IN_RENDERPASS,
  COMMAND_BUFFER_STATE_RECORDING_ENDED,
  COMMAND_BUFFER_STATE_SUBMITTED,
  COMMAND_BUFFER_STATE_NOT_ALLOCATED
};

struct vulkan_command_buffer {
  VkCommandBuffer handle;

  vulkan_command_buffer_state state;
};

struct vulkan_fence {
  VkFence handle;
  bool is_signaled;
};

struct vulkan_context {
  u32 framebuffer_width;
  u32 framebuffer_height;
  u64 framebuffer_size_generation;
  u64 framebuffer_size_last_generation;
  VkInstance instance;
  VkAllocationCallbacks *allocator;
  VkSurfaceKHR surface;

#ifdef LAI_DEBUG
  VkDebugUtilsMessengerEXT debug_messenger;
#endif

  vulkan_device device;

  vulkan_swapchain swapchain;
  vulkan_renderpass main_renderpass;

  vulkan_command_buffer *graphics_command_buffers;

  VkSemaphore *image_available_semaphores;
  VkSemaphore *queue_complete_semaphores;

  u32 in_flight_fence_count;
  vulkan_fence *in_flight_fences;
  vulkan_fence **images_in_flight;

  u32 image_index;
  u32 current_frame;

  bool recreating_swapchain;

  i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
};