#include "renderer/vulkan/shaders/vulkan_object_shader.h"
#include "renderer/vulkan/shaders/vulkan_shader_utils.h"
#include "renderer/vulkan/vulkan_pipeline.h"

#include "math/math_types.h"

#include "base/lai_memory.h"
#include "base/log.h"

#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

bool vulkan_object_shader_create(vulkan_context *context,
                                 vulkan_object_shader *shader) {
  char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
  VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {
      VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

  for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
    if (!create_shader_module(context, BUILTIN_SHADER_NAME_OBJECT,
                              stage_type_strs[i], stage_types[i], i,
                              shader->stages)) {
      LAI_LOG_ERROR("Unable to create %s shader module for %s",
                    stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
      return false;
    }
  }

  // TODO: Descriptors

  // Pipeline creation
  VkViewport viewport;
  viewport.x = 0.0f;
  viewport.y = (f32)context->framebuffer_height;
  viewport.width = (f32)context->framebuffer_width;
  viewport.height = -(f32)context->framebuffer_height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Scissor
  VkRect2D scissor;
  scissor.offset.x = scissor.offset.y = 0;
  scissor.extent.width = context->framebuffer_width;
  scissor.extent.height = context->framebuffer_height;

  // Attributes
  u32 offset = 0;
  const i32 attribute_count = 1;
  VkVertexInputAttributeDescription attribute_descriptions[attribute_count];
  // Position
  VkFormat formats[attribute_count] = {VK_FORMAT_R32G32B32_SFLOAT};
  u64 sizes[attribute_count] = {sizeof(vec3)};
  for (u32 i = 0; i < attribute_count; ++i) {
    attribute_descriptions[i].binding = 0;
    attribute_descriptions[i].location = i;
    attribute_descriptions[i].format = formats[i];
    attribute_descriptions[i].offset = offset;

    offset += sizes[i];
  }

  // Stages
  VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT];
  lai_zero_memory(stage_create_infos, sizeof(stage_create_infos));

  for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
    stage_create_infos[i].sType = shader->stages[i].stage_create_info.sType;
    stage_create_infos[i] = shader->stages[i].stage_create_info;
  }

  if (!vulkan_graphics_pipeline_create(
          context, &context->main_renderpass, attribute_count,
          attribute_descriptions, 0, nullptr, OBJECT_SHADER_STAGE_COUNT,
          stage_create_infos, viewport, scissor, false, &shader->pipeline)) {
    LAI_LOG_ERROR("Failed to load graphics pipeline for object shader");
    return false;
  }

  return true;
}

void vulkan_object_shader_destroy(vulkan_context *context,
                                  vulkan_object_shader *shader) {
  vulkan_pipeline_destroy(context, &context->object_shader.pipeline);

  for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
    vkDestroyShaderModule(context->device.logical_device,
                          shader->stages[i].handle, context->allocator);
    shader->stages[i].handle = nullptr;
  }
}

void vulkan_object_shader_use(vulkan_context *context,
                              vulkan_object_shader *shader) {}
