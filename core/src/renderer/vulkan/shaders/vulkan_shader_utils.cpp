#include "renderer/vulkan/shaders/vulkan_shader_utils.h"

#include "base/lai_memory.h"
#include "base/lai_string.h"
#include "base/log.h"

#include "platform/filesystem.h"

bool create_shader_module(vulkan_context *context, const char *name,
                          const char *type_str,
                          VkShaderStageFlagBits shader_stage_flag,
                          u32 stage_index, vulkan_shader_stage *shader_stages) {
  char filename[512];
  string_format(filename, "assets/shaders/%s.%s.spv", name, type_str);

  lai_zero_memory(&shader_stages[stage_index].create_info,
                  sizeof(VkShaderModuleCreateInfo));
  shader_stages[stage_index].create_info.sType =
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

  // TODO: file input
  file_handle handle;
  if (!filesystem_open(filename, FILE_MODE_READ, true, &handle)) {
    LAI_LOG_ERROR("Could not open the file: %s", filename);
    return false;
  }

  u64 size = 0;
  u8 *file_buffer = 0;
  if (!filesystem_read_all_bytes(&handle, &file_buffer, &size)) {
    LAI_LOG_ERROR("Could not read the file: %s", filename);
    return false;
  }
  shader_stages[stage_index].create_info.codeSize = size;
  shader_stages[stage_index].create_info.pCode = (u32 *)file_buffer;

  filesystem_close(&handle);

  VK_CHECK(vkCreateShaderModule(
      context->device.logical_device, &shader_stages[stage_index].create_info,
      context->allocator, &shader_stages[stage_index].handle));

  lai_zero_memory(&shader_stages[stage_index].stage_create_info,
                  sizeof(VkPipelineShaderStageCreateInfo));
  shader_stages[stage_index].stage_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stages[stage_index].stage_create_info.stage = shader_stage_flag;
  shader_stages[stage_index].stage_create_info.module =
      shader_stages[stage_index].handle;
  shader_stages[stage_index].stage_create_info.pName = "main";

  if (file_buffer) {
    lai_free(file_buffer, sizeof(u8) * size, MEMORY_TAG_STRING);
    file_buffer = 0;
  }

  return true;
}