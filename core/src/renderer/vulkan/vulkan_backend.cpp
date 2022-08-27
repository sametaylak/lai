#include "renderer/vulkan/vulkan_backend.h"
#include "renderer/vulkan/vulkan_command_buffer.h"
#include "renderer/vulkan/vulkan_device.h"
#include "renderer/vulkan/vulkan_fence.h"
#include "renderer/vulkan/vulkan_framebuffer.h"
#include "renderer/vulkan/vulkan_platform.h"
#include "renderer/vulkan/vulkan_renderpass.h"
#include "renderer/vulkan/vulkan_swapchain.h"
#include "renderer/vulkan/vulkan_types.inl"

#include "base/application.h"

#include "base/lai_string.h"
#include "base/log.h"

#include "containers/darray.h"

#include "platform/platform.h"

static vulkan_context context;
static u32 cached_framebuffer_width = 0;
static u32 cached_framebuffer_height = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data);

i32 find_memory_index(u32 type_filter, u32 property_flags);

void create_command_buffers(renderer_backend *backend);
void regenerate_framebuffers(renderer_backend *backend,
                             vulkan_swapchain *swapchain,
                             vulkan_renderpass *renderpass);

bool vulkan_renderer_backend_initialize(renderer_backend *backend,
                                        const char *application_name,
                                        struct platform_state *plat_state) {
  context.find_memory_index = find_memory_index;
  context.allocator = nullptr;

  application_get_framebuffer_size(&cached_framebuffer_width,
                                   &cached_framebuffer_height);
  context.framebuffer_width =
      cached_framebuffer_width != 0 ? cached_framebuffer_width : 800;
  context.framebuffer_height =
      cached_framebuffer_height != 0 ? cached_framebuffer_height : 600;
  cached_framebuffer_width = 0;
  cached_framebuffer_height = 0;

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_2;
  app_info.pApplicationName = application_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Lai Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  const char **required_extensions = darray_create(const char *);
  darray_push(required_extensions,
              VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension
  platform_get_required_extension_names(
      &required_extensions); // Platform specific extension such as metal

#ifdef LAI_DEBUG
  darray_push(required_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  LAI_LOG_DEBUG("required_extensions");
  u32 length = darray_length(required_extensions);
  for (u32 i = 0; i < length; ++i) {
    LAI_LOG_DEBUG(required_extensions[i]);
  }
#endif

  create_info.enabledExtensionCount = darray_length(required_extensions);
  create_info.ppEnabledExtensionNames = required_extensions;

  const char **required_validation_layers = darray_create(const char *);

#ifdef LAI_DEBUG
  LAI_LOG_DEBUG("Validation layers enabled!");

  darray_push(required_validation_layers, "VK_LAYER_KHRONOS_validation");

  u32 available_layer_count = 0;
  VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
  VkLayerProperties *available_layers =
      darray_reserve(VkLayerProperties, available_layer_count);
  VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count,
                                              available_layers));

  for (u32 i = 0; i < darray_length(required_validation_layers); ++i) {
    LAI_LOG_DEBUG("Searching for layer: %s...", required_validation_layers[i]);
    bool found = false;
    for (u32 j = 0; j < available_layer_count; ++j) {
      if (strings_equal(required_validation_layers[i],
                        available_layers[j].layerName)) {
        found = true;
        LAI_LOG_DEBUG("Found.");
        break;
      }
    }

    if (!found) {
      LAI_LOG_FATAL("Required validation layer is not found: %s",
                    required_validation_layers[i]);
      return false;
    }
  }
  LAI_LOG_DEBUG("All required validation layers are present");
#endif

  create_info.enabledLayerCount = darray_length(required_validation_layers);
  create_info.ppEnabledLayerNames = required_validation_layers;

  VK_CHECK(
      vkCreateInstance(&create_info, context.allocator, &context.instance));
  LAI_LOG_DEBUG("Vulkan instance created!");

#ifdef LAI_DEBUG
  LAI_LOG_DEBUG("Creating vulkan debugger");
  u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

  VkDebugUtilsMessengerCreateInfoEXT debugger_create_info = {};
  debugger_create_info.sType =
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugger_create_info.messageSeverity = log_severity;
  debugger_create_info.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  debugger_create_info.pfnUserCallback = vk_debug_callback;

  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          context.instance, "vkCreateDebugUtilsMessengerEXT");
  LAI_ASSERT_MESSAGE(func, "Failed to create debug messenger!");
  VK_CHECK(func(context.instance, &debugger_create_info, context.allocator,
                &context.debug_messenger));
  LAI_LOG_DEBUG("Vulkan debugger created!");
#endif

  LAI_LOG_DEBUG("Creating vulkan surface");
  if (!platform_create_vulkan_surface(plat_state, &context)) {
    LAI_LOG_FATAL("Failed to create vulkan surface!");
    return false;
  }

  if (!vulkan_device_create(&context)) {
    LAI_LOG_FATAL("Failed to create vulkan device!");
    return false;
  }

  vulkan_swapchain_create(&context, context.framebuffer_width,
                          context.framebuffer_height, &context.swapchain);

  vulkan_renderpass_create(
      &context, &context.main_renderpass, 0, 0, context.framebuffer_width,
      context.framebuffer_height, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0);

  // Swapchain framebuffer
  context.swapchain.framebuffers =
      darray_reserve(vulkan_framebuffer, context.swapchain.image_count);
  regenerate_framebuffers(backend, &context.swapchain,
                          &context.main_renderpass);

  // Create command buffers
  create_command_buffers(backend);

  // Create sync objects
  context.image_available_semaphores =
      darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
  context.queue_complete_semaphores =
      darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
  context.in_flight_fences =
      darray_reserve(vulkan_fence, context.swapchain.max_frames_in_flight);

  for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(context.device.logical_device, &semaphore_create_info,
                      context.allocator,
                      &context.image_available_semaphores[i]);
    vkCreateSemaphore(context.device.logical_device, &semaphore_create_info,
                      context.allocator, &context.queue_complete_semaphores[i]);

    vulkan_fence_create(&context, true, &context.in_flight_fences[i]);
  }

  context.images_in_flight =
      darray_reserve(vulkan_fence *, context.swapchain.image_count);
  for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    context.images_in_flight[i] = nullptr;
  }

  LAI_LOG_INFO("Vulkan renderer created!");
  return true;
}

void vulkan_renderer_backend_shutdown(renderer_backend *backend) {
  vkDeviceWaitIdle(context.device.logical_device);

  for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
    if (context.image_available_semaphores[i]) {
      vkDestroySemaphore(context.device.logical_device,
                         context.image_available_semaphores[i],
                         context.allocator);
      context.image_available_semaphores[i] = nullptr;
    }

    if (context.queue_complete_semaphores[i]) {
      vkDestroySemaphore(context.device.logical_device,
                         context.queue_complete_semaphores[i],
                         context.allocator);
      context.queue_complete_semaphores[i] = nullptr;
    }

    vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
  }

  {
    darray_destroy(context.image_available_semaphores);
    context.image_available_semaphores = nullptr;

    darray_destroy(context.queue_complete_semaphores);
    context.queue_complete_semaphores = nullptr;

    darray_destroy(context.in_flight_fences);
    context.in_flight_fences = nullptr;

    darray_destroy(context.images_in_flight);
    context.images_in_flight = nullptr;
  }

  // Destroy command buffers
  for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    if (context.graphics_command_buffers[i].handle) {
      vulkan_command_buffer_free(&context, context.device.graphics_command_pool,
                                 &context.graphics_command_buffers[i]);

      context.graphics_command_buffers[i].handle = nullptr;
    }
  }
  darray_destroy(context.graphics_command_buffers);
  context.graphics_command_buffers = nullptr;

  // Destroy framebuffers
  for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
  }

  vulkan_renderpass_destroy(&context, &context.main_renderpass);

  vulkan_swapchain_destroy(&context, &context.swapchain);

  vulkan_device_destroy(&context);

  LAI_LOG_DEBUG("Destroying vulkan surface");
  if (context.surface) {
    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
    context.surface = nullptr;
  }

  LAI_LOG_DEBUG("Destroying vulkan debugger");
  if (context.debug_messenger) {
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(context.instance, context.debug_messenger, context.allocator);
  }

  LAI_LOG_DEBUG("Destroying vulkan instance");
  vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_on_resized(renderer_backend *backend, u16 width,
                                        u16 height) {}

bool vulkan_renderer_backend_begin_frame(renderer_backend *backend,
                                         f32 delta_time) {
  return true;
}

bool vulkan_renderer_backend_end_frame(renderer_backend *backend,
                                       f32 delta_time) {
  return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                  VkDebugUtilsMessageTypeFlagsEXT message_types,
                  const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                  void *user_data) {
  switch (message_severity) {
  default:
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    LAI_LOG_ERROR(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    LAI_LOG_WARN(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    LAI_LOG_INFO(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    LAI_LOG_TRACE(callback_data->pMessage);
    break;
  }
  return VK_FALSE;
}

i32 find_memory_index(u32 type_filter, u32 property_flags) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(context.device.physical_device,
                                      &memory_properties);

  for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
    if (type_filter & (1 << i) &&
        (memory_properties.memoryTypes[i].propertyFlags & property_flags) ==
            property_flags) {
      return i;
    }
  }

  return -1;
}

void create_command_buffers(renderer_backend *backend) {
  if (!context.graphics_command_buffers) {
    context.graphics_command_buffers =
        darray_reserve(vulkan_command_buffer, context.swapchain.image_count);
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
      lai_zero_memory(&context.graphics_command_buffers[i],
                      sizeof(vulkan_command_buffer));
    }
  }

  for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    if (context.graphics_command_buffers[i].handle) {
      vulkan_command_buffer_free(&context, context.device.graphics_command_pool,
                                 &context.graphics_command_buffers[i]);
    }

    lai_zero_memory(&context.graphics_command_buffers[i],
                    sizeof(vulkan_command_buffer));

    vulkan_command_buffer_allocate(&context,
                                   context.device.graphics_command_pool, true,
                                   &context.graphics_command_buffers[i]);
  }

  LAI_LOG_INFO("Vulkan command buffers created!");
}

void regenerate_framebuffers(renderer_backend *backend,
                             vulkan_swapchain *swapchain,
                             vulkan_renderpass *renderpass) {
  for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    u32 attachment_count = 2;
    VkImageView attachments[] = {swapchain->views[i],
                                 swapchain->depth_attachment.view};

    vulkan_framebuffer_create(&context, renderpass, context.framebuffer_width,
                              context.framebuffer_height, attachment_count,
                              attachments, &context.swapchain.framebuffers[i]);
  }
}