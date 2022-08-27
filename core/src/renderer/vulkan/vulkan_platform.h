#pragma once

#include "defines.h"

bool platform_create_vulkan_surface(struct platform_state *plat_state,
                                    struct vulkan_context *context);

void platform_get_required_extension_names(const char ***names_darray);
