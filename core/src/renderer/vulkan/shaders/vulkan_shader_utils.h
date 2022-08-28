#pragma once

#include "renderer/vulkan/vulkan_types.inl"

bool create_shader_module(vulkan_context *context, const char *name,
                          const char *type_str,
                          VkShaderStageFlagBits shader_stage_flag,
                          u32 stage_index, vulkan_shader_stage *shader_stages);