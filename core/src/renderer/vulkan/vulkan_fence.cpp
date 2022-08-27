#include "renderer/vulkan/vulkan_fence.h"
#include "base/log.h"

void vulkan_fence_create(vulkan_context *context, bool create_signaled,
                         vulkan_fence *out_fence) {

  out_fence->is_signaled = create_signaled;

  VkFenceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  if (out_fence->is_signaled) {
    create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  }

  VK_CHECK(vkCreateFence(context->device.logical_device, &create_info,
                         context->allocator, &out_fence->handle));
}

void vulkan_fence_destroy(vulkan_context *context, vulkan_fence *out_fence) {
  if (out_fence->handle) {
    vkDestroyFence(context->device.logical_device, out_fence->handle,
                   context->allocator);
    out_fence->handle = nullptr;
  }
  out_fence->is_signaled = false;
}

bool vulkan_fence_wait(vulkan_context *context, vulkan_fence *fence,
                       u64 timeout_ns) {
  if (!fence->is_signaled) {
    VkResult result = vkWaitForFences(context->device.logical_device, 1,
                                      &fence->handle, true, timeout_ns);

    switch (result) {
    case VK_SUCCESS:
      fence->is_signaled = true;
      return true;
    case VK_TIMEOUT:
      LAI_LOG_WARN("vk_fence_wait - Timed out");
      break;
    case VK_ERROR_DEVICE_LOST:
      LAI_LOG_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST");
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      LAI_LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      LAI_LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY");
      break;
    default:
      LAI_LOG_ERROR("vk_fence_wait - An unknown error has occurred");
      break;
    }
  } else {
    return true;
  }
  return false;
}

void vulkan_fence_reset(vulkan_context *context, vulkan_fence *fence) {
  if (fence->is_signaled) {
    VK_CHECK(vkResetFences(context->device.logical_device, 1, &fence->handle));
    fence->is_signaled = false;
  }
}