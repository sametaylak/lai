#include "renderer/vulkan/vulkan_device.h"

#include "base/lai_memory.h"
#include "base/lai_string.h"
#include "base/log.h"

#include "containers/darray.h"

struct vulkan_physical_device_requirements {
  bool graphics;
  bool present;
  bool compute;
  bool transfer;
  const char **device_extension_names;
  bool sampler_anisotropy;
  bool discrete_gpu;
};

struct vulkan_physical_device_queue_family_info {
  u32 graphics_family_index;
  u32 present_family_index;
  u32 compute_family_index;
  u32 transfer_family_index;
};

bool select_physical_device(vulkan_context *context);
bool physical_device_meets_requirements(
    VkPhysicalDevice device, VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties *properties,
    const VkPhysicalDeviceFeatures *features,
    const vulkan_physical_device_requirements *requirements,
    vulkan_physical_device_queue_family_info *out_queue_family_info,
    vulkan_swapchain_support_info *out_swapchain_support);

bool vulkan_device_create(vulkan_context *context) {
  if (!select_physical_device(context)) {
    return false;
  }

  LAI_LOG_INFO("Creating logical device...");

  bool present_shares_graphics_queue = context->device.graphics_queue_index ==
                                       context->device.present_queue_index;
  bool transfer_shares_graphics_queue = context->device.graphics_queue_index ==
                                        context->device.transfer_queue_index;

  u32 index_count = 1;
  if (!present_shares_graphics_queue) {
    index_count++;
  }
  if (!transfer_shares_graphics_queue) {
    index_count++;
  }

  u32 indices[index_count];
  u8 index = 0;
  indices[index++] = context->device.graphics_queue_index;
  if (!present_shares_graphics_queue) {
    indices[index++] = context->device.present_queue_index;
  }
  if (!transfer_shares_graphics_queue) {
    indices[index++] = context->device.transfer_queue_index;
  }

  VkDeviceQueueCreateInfo queue_create_infos[index_count];
  for (u32 i = 0; i < index_count; ++i) {
    queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[i].queueFamilyIndex = indices[i];
    queue_create_infos[i].queueCount = 1;
    queue_create_infos[i].flags = 0;
    queue_create_infos[i].pNext = nullptr;

    f32 queue_priority = 1.0f;
    queue_create_infos[i].pQueuePriorities = &queue_priority;
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.samplerAnisotropy = VK_TRUE; // Request this feature

  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = index_count;
  device_create_info.pQueueCreateInfos = queue_create_infos;
  device_create_info.pEnabledFeatures = &device_features;

  const char *extension_names[2];
  extension_names[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  extension_names[1] = "VK_KHR_portability_subset";
  // { VK_KHR_SWAPCHAIN_EXTENSION_NAME, &"VK_KHR_portability_subset" }
  device_create_info.enabledExtensionCount = 2;
  device_create_info.ppEnabledExtensionNames = extension_names;

  device_create_info.enabledLayerCount = 0;
  device_create_info.ppEnabledLayerNames = nullptr;

  VK_CHECK(vkCreateDevice(context->device.physical_device, &device_create_info,
                          context->allocator, &context->device.logical_device));

  LAI_LOG_INFO("Logical device created!");

  vkGetDeviceQueue(context->device.logical_device,
                   context->device.graphics_queue_index, 0,
                   &context->device.graphics_queue);

  vkGetDeviceQueue(context->device.logical_device,
                   context->device.present_queue_index, 0,
                   &context->device.present_queue);

  vkGetDeviceQueue(context->device.logical_device,
                   context->device.transfer_queue_index, 0,
                   &context->device.transfer_queue);

  LAI_LOG_INFO("Queues obtained!");

  return true;
}

void vulkan_device_destroy(vulkan_context *context) {
  context->device.graphics_queue = nullptr;
  context->device.present_queue = nullptr;
  context->device.transfer_queue = nullptr;

  LAI_LOG_INFO("Destroying logical device resources...");
  if (context->device.logical_device) {
    vkDestroyDevice(context->device.logical_device, context->allocator);
    context->device.logical_device = nullptr;
  }

  LAI_LOG_INFO("Releasing physical device resources...");
  context->device.physical_device = nullptr;

  if (context->device.swapchain_support.formats) {
    lai_free(context->device.swapchain_support.formats,
             sizeof(VkSurfaceFormatKHR) *
                 context->device.swapchain_support.format_count,
             MEMORY_TAG_RENDERER);
    context->device.swapchain_support.formats = nullptr;
    context->device.swapchain_support.format_count = 0;
  }

  if (context->device.swapchain_support.present_modes) {
    lai_free(context->device.swapchain_support.present_modes,
             sizeof(VkPresentModeKHR) *
                 context->device.swapchain_support.present_mode_count,
             MEMORY_TAG_RENDERER);
    context->device.swapchain_support.present_modes = nullptr;
    context->device.swapchain_support.present_mode_count = 0;
  }

  lai_zero_memory(&context->device.swapchain_support.capabilities,
                  sizeof(context->device.swapchain_support.capabilities));

  context->device.graphics_queue_index = -1;
  context->device.present_queue_index = -1;
  context->device.transfer_queue_index = -1;
}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device, VkSurfaceKHR surface,
    vulkan_swapchain_support_info *out_support_info) {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      physical_device, surface, &out_support_info->capabilities));
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      physical_device, surface, &out_support_info->format_count, 0));

  if (out_support_info->format_count != 0) {
    if (!out_support_info->formats) {
      out_support_info->formats = (VkSurfaceFormatKHR *)lai_allocate(
          sizeof(VkSurfaceFormatKHR) * out_support_info->format_count,
          MEMORY_TAG_RENDERER);
    }
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, surface, &out_support_info->format_count,
        out_support_info->formats));
  }

  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
      physical_device, surface, &out_support_info->present_mode_count, 0));
  if (out_support_info->present_mode_count != 0) {
    if (!out_support_info->present_modes) {
      out_support_info->present_modes = (VkPresentModeKHR *)lai_allocate(
          sizeof(VkPresentModeKHR) * out_support_info->present_mode_count,
          MEMORY_TAG_RENDERER);
    }
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface, &out_support_info->present_mode_count,
        out_support_info->present_modes));
  }
}

bool vulkan_device_detect_depth_format(vulkan_device *device) {
  const u64 candidate_count = 3;
  VkFormat candidates[candidate_count] = {VK_FORMAT_D32_SFLOAT,
                                          VK_FORMAT_D32_SFLOAT_S8_UINT,
                                          VK_FORMAT_D24_UNORM_S8_UINT};

  u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  for (u64 i = 0; i < candidate_count; ++i) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i],
                                        &properties);

    if ((properties.linearTilingFeatures & flags) == flags) {
      device->depth_format = candidates[i];
      return true;
    } else if ((properties.optimalTilingFeatures & flags) == flags) {
      device->depth_format = candidates[i];
      return true;
    }
  }
  return false;
}

bool select_physical_device(vulkan_context *context) {
  u32 physical_device_count = 0;
  VK_CHECK(
      vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));
  if (physical_device_count == 0) {
    LAI_LOG_FATAL("No device found");
    return false;
  }

  VkPhysicalDevice physical_devices[physical_device_count];
  VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count,
                                      physical_devices));
  for (u32 i = 0; i < physical_device_count; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

    VkPhysicalDeviceMemoryProperties memory;
    vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

    vulkan_physical_device_requirements requirements = {};
    requirements.graphics = true;
    requirements.present = true;
    requirements.transfer = true;
    requirements.sampler_anisotropy = true;
    requirements.discrete_gpu = false;
    requirements.device_extension_names = darray_create(const char *);
    darray_push(requirements.device_extension_names,
                VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    vulkan_physical_device_queue_family_info queue_info = {};
    bool result = physical_device_meets_requirements(
        physical_devices[i], context->surface, &properties, &features,
        &requirements, &queue_info, &context->device.swapchain_support);

    if (result) {
      LAI_LOG_INFO("Selected device: %s", properties.deviceName);
      LAI_LOG_INFO("GPU Driver Version: %d.%d.%d",
                   VK_VERSION_MAJOR(properties.driverVersion),
                   VK_VERSION_MAJOR(properties.driverVersion),
                   VK_VERSION_MAJOR(properties.driverVersion));
      LAI_LOG_INFO("Vulkan API Version: %d.%d.%d",
                   VK_VERSION_MAJOR(properties.apiVersion),
                   VK_VERSION_MAJOR(properties.apiVersion),
                   VK_VERSION_MAJOR(properties.apiVersion));

      context->device.physical_device = physical_devices[i];
      context->device.graphics_queue_index = queue_info.graphics_family_index;
      context->device.present_queue_index = queue_info.present_family_index;
      context->device.transfer_queue_index = queue_info.transfer_family_index;

      context->device.properties = properties;
      context->device.features = features;
      context->device.memory = memory;

      break;
    }
  }

  if (!context->device.physical_device) {
    LAI_LOG_FATAL("No physical device found, shutting down...");
    return false;
  }

  return true;
}

bool physical_device_meets_requirements(
    VkPhysicalDevice device, VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties *properties,
    const VkPhysicalDeviceFeatures *features,
    const vulkan_physical_device_requirements *requirements,
    vulkan_physical_device_queue_family_info *out_queue_family_info,
    vulkan_swapchain_support_info *out_swapchain_support) {

  out_queue_family_info->graphics_family_index = -1;
  out_queue_family_info->present_family_index = -1;
  out_queue_family_info->compute_family_index = -1;
  out_queue_family_info->transfer_family_index = -1;

  if (requirements->discrete_gpu) {
    if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      LAI_LOG_INFO("Device is not a discrete gpu. Skipping");
      return false;
    }
  }

  u32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
  VkQueueFamilyProperties queue_families[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families);

  LAI_LOG_DEBUG("Graphics | Present | Compute | Transfer | Name");
  u8 min_transfer_score = 255;
  for (u32 i = 0; i < queue_family_count; ++i) {
    u8 current_transfer_score = 0;

    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      out_queue_family_info->graphics_family_index = i;
      ++current_transfer_score;
    }

    if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      out_queue_family_info->compute_family_index = i;
      ++current_transfer_score;
    }

    if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      if (current_transfer_score <= min_transfer_score) {
        min_transfer_score = current_transfer_score;
        out_queue_family_info->transfer_family_index = i;
      }
    }

    VkBool32 supports_present = VK_FALSE;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                  &supports_present));
    if (supports_present) {
      out_queue_family_info->present_family_index = i;
    }
  }

  LAI_LOG_DEBUG("       %d |       %d |       %d |        %d | %s",
                out_queue_family_info->graphics_family_index != -1,
                out_queue_family_info->present_family_index != -1,
                out_queue_family_info->compute_family_index != -1,
                out_queue_family_info->transfer_family_index != -1,
                properties->deviceName);

  if ((!requirements->graphics ||
       (requirements->graphics &&
        out_queue_family_info->graphics_family_index != -1)) &&
      (!requirements->present ||
       (requirements->present &&
        out_queue_family_info->present_family_index != -1)) &&
      (!requirements->compute ||
       (requirements->compute &&
        out_queue_family_info->compute_family_index != -1)) &&
      (!requirements->transfer ||
       (requirements->transfer &&
        out_queue_family_info->transfer_family_index != -1))) {
    LAI_LOG_DEBUG("Device meets queue requirements");

    vulkan_device_query_swapchain_support(device, surface,
                                          out_swapchain_support);

    if (out_swapchain_support->format_count < 1 ||
        out_swapchain_support->present_mode_count < 1) {
      if (out_swapchain_support->formats) {
        lai_free(out_swapchain_support->formats,
                 sizeof(VkSurfaceFormatKHR) *
                     out_swapchain_support->format_count,
                 MEMORY_TAG_RENDERER);
      }
      if (out_swapchain_support->present_modes) {
        lai_free(out_swapchain_support->present_modes,
                 sizeof(VkPresentModeKHR) *
                     out_swapchain_support->present_mode_count,
                 MEMORY_TAG_RENDERER);
      }
      LAI_LOG_INFO(
          "Required swapchain support not present, skipping device...");
      return false;
    }

    if (requirements->device_extension_names) {
      u32 available_extension_count = 0;
      VkExtensionProperties *available_extensions = nullptr;
      VK_CHECK(vkEnumerateDeviceExtensionProperties(
          device, nullptr, &available_extension_count, nullptr));
      if (available_extension_count != 0) {
        available_extensions = (VkExtensionProperties *)lai_allocate(
            sizeof(VkExtensionProperties) * available_extension_count,
            MEMORY_TAG_RENDERER);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(
            device, nullptr, &available_extension_count, available_extensions));

        u32 required_extension_count =
            darray_length(requirements->device_extension_names);
        for (u32 i = 0; i < required_extension_count; ++i) {
          bool found = false;
          for (u32 j = 0; j < available_extension_count; ++j) {
            if (strings_equal(requirements->device_extension_names[i],
                              available_extensions[j].extensionName)) {
              found = true;
              break;
            }
          }

          if (!found) {
            LAI_LOG_DEBUG("Required extensions not found. Skipping device...");
            lai_free(available_extensions,
                     sizeof(VkExtensionProperties) * available_extension_count,
                     MEMORY_TAG_RENDERER);
          }
        }
      }
      lai_free(available_extensions,
               sizeof(VkExtensionProperties) * available_extension_count,
               MEMORY_TAG_RENDERER);
    }

    if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
      LAI_LOG_INFO("Device does not support samplerAnisotropy, skipping...");
      return false;
    }

    return true;
  }

  return false;
}
