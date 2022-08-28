#include "renderer/vulkan/vulkan_pipeline.h"
#include "renderer/vulkan/vulkan_utils.h"

#include "math/math_types.h"

#include "base/lai_memory.h"
#include "base/log.h"

bool vulkan_graphics_pipeline_create(
    vulkan_context *context, vulkan_renderpass *renderpass, u32 attribute_count,
    VkVertexInputAttributeDescription *attributes,
    u32 descriptor_set_layout_count,
    VkDescriptorSetLayout *descriptor_set_layouts, u32 stage_count,
    VkPipelineShaderStageCreateInfo *stages, VkViewport viewport,
    VkRect2D scissor, bool is_wireframe, vulkan_pipeline *out_pipeline) {

  // Viewport state
  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterized_create_info = {};
  rasterized_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterized_create_info.depthClampEnable = VK_FALSE;
  rasterized_create_info.rasterizerDiscardEnable = VK_FALSE;
  rasterized_create_info.polygonMode =
      is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
  rasterized_create_info.lineWidth = 1.0f;
  rasterized_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterized_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterized_create_info.depthBiasEnable = VK_FALSE;
  rasterized_create_info.depthBiasConstantFactor = 0.0f;
  rasterized_create_info.depthBiasClamp = 0.0f;
  rasterized_create_info.depthBiasSlopeFactor = 0.0f;

  // Multisampling
  VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
  multisample_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_create_info.sampleShadingEnable = VK_FALSE;
  multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisample_create_info.minSampleShading = 1.0f;
  multisample_create_info.pSampleMask = nullptr;
  multisample_create_info.alphaToCoverageEnable = VK_FALSE;
  multisample_create_info.alphaToOneEnable = VK_FALSE;

  // Depth and Stencil
  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
  lai_zero_memory(&color_blend_attachment_state,
                  sizeof(VkPipelineColorBlendAttachmentState));

  color_blend_attachment_state.blendEnable = VK_TRUE;
  color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_state.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_state.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

  color_blend_attachment_state.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
  color_blend_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_state_create_info.attachmentCount = 1;
  color_blend_state_create_info.pAttachments = &color_blend_attachment_state;

  // Dynamic state
  const u32 dynamic_state_count = 3;
  VkDynamicState dynamic_states[3] = {VK_DYNAMIC_STATE_VIEWPORT,
                                      VK_DYNAMIC_STATE_SCISSOR,
                                      VK_DYNAMIC_STATE_LINE_WIDTH};

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
  dynamic_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.dynamicStateCount = dynamic_state_count;
  dynamic_state_create_info.pDynamicStates = dynamic_states;

  // Vertex input
  VkVertexInputBindingDescription binding_description;
  binding_description.binding = 0; // Binding index
  binding_description.stride = sizeof(vertex_3d);
  binding_description.inputRate =
      VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex.

  // Attributes
  VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.vertexAttributeDescriptionCount = attribute_count;
  vertex_input_info.pVertexAttributeDescriptions = attributes;

  // Input assembly
  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  // Pipeline layout
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
  pipeline_layout_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  // Push constants
  // if (push_constant_range_count > 0) {
  //   if (push_constant_range_count > 32) {
  //     LAI_LOG_ERROR("vulkan_graphics_pipeline_create: cannot have more than
  //     32 push "
  //            "constant ranges. Passed count: %i",
  //            push_constant_range_count);
  //     return false;
  //   }

  //   // NOTE: 32 is the max number of ranges we can ever have, since spec only
  //   // guarantees 128 bytes with 4-byte alignment.
  //   VkPushConstantRange ranges[32];
  //   kzero_memory(ranges, sizeof(VkPushConstantRange) * 32);
  //   for (u32 i = 0; i < push_constant_range_count; ++i) {
  //     ranges[i].stageFlags =
  //         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  //     ranges[i].offset = push_constant_ranges[i].offset;
  //     ranges[i].size = push_constant_ranges[i].size;
  //   }
  //   pipeline_layout_create_info.pushConstantRangeCount =
  //       push_constant_range_count;
  //   pipeline_layout_create_info.pPushConstantRanges = ranges;
  // } else {
  //   pipeline_layout_create_info.pushConstantRangeCount = 0;
  //   pipeline_layout_create_info.pPushConstantRanges = 0;
  // }

  // Descriptor set layouts
  pipeline_layout_create_info.setLayoutCount = descriptor_set_layout_count;
  pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts;

  // Create the pipeline layout.
  VK_CHECK(vkCreatePipelineLayout(context->device.logical_device,
                                  &pipeline_layout_create_info,
                                  context->allocator, &out_pipeline->layout));

  // Pipeline create
  VkGraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = stage_count;
  pipeline_create_info.pStages = stages;
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly;

  pipeline_create_info.pViewportState = &viewport_state;
  pipeline_create_info.pRasterizationState = &rasterized_create_info;
  pipeline_create_info.pMultisampleState = &multisample_create_info;
  pipeline_create_info.pDepthStencilState = &depth_stencil;
  pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
  pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  pipeline_create_info.pTessellationState = nullptr;

  pipeline_create_info.layout = out_pipeline->layout;

  pipeline_create_info.renderPass = renderpass->handle;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  VkResult result = vkCreateGraphicsPipelines(
      context->device.logical_device, VK_NULL_HANDLE, 1, &pipeline_create_info,
      context->allocator, &out_pipeline->handle);

  if (vulkan_result_is_success(result)) {
    LAI_LOG_INFO("Graphics pipeline created!");
    return true;
  }

  LAI_LOG_ERROR("vkCreateGraphicsPipelines failed with %s.",
                vulkan_result_string(result, true));
  return false;
}

void vulkan_pipeline_destroy(vulkan_context *context,
                             vulkan_pipeline *pipeline) {
  if (pipeline) {
    // Destroy pipeline
    if (pipeline->handle) {
      vkDestroyPipeline(context->device.logical_device, pipeline->handle,
                        context->allocator);
      pipeline->handle = 0;
    }

    // Destroy layout
    if (pipeline->layout) {
      vkDestroyPipelineLayout(context->device.logical_device, pipeline->layout,
                              context->allocator);
      pipeline->layout = nullptr;
    }
  }
}

void vulkan_pipeline_bind(vulkan_command_buffer *command_buffer,
                          VkPipelineBindPoint bind_point,
                          vulkan_pipeline *pipeline) {
  vkCmdBindPipeline(command_buffer->handle, bind_point, pipeline->handle);
}