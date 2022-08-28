#pragma once

#include "renderer/renderer_types.inl"
#include "renderer/vulkan/vulkan_types.inl"

bool vulkan_object_shader_create(vulkan_context *context,
                                 vulkan_object_shader *shader);
void vulkan_object_shader_destroy(vulkan_context *context,
                                  vulkan_object_shader *shader);
void vulkan_object_shader_use(vulkan_context *context,
                              vulkan_object_shader *shader);