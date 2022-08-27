#pragma once

#include "renderer/vulkan/vulkan_types.inl"

const char *vulkan_result_string(VkResult result, bool get_extended);

bool vulkan_result_is_success(VkResult result);