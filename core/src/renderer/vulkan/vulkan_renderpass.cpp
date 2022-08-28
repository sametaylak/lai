#include "renderer/vulkan/vulkan_renderpass.h"
#include "base/lai_memory.h"

void vulkan_renderpass_create(vulkan_context *context,
                              vulkan_renderpass *out_renderpass, f32 x, f32 y,
                              f32 w, f32 h, f32 r, f32 g, f32 b, f32 a,
                              f32 depth, u32 stencil) {

  out_renderpass->x = x;
  out_renderpass->y = y;
  out_renderpass->w = w;
  out_renderpass->h = h;

  out_renderpass->r = r;
  out_renderpass->g = g;
  out_renderpass->b = b;
  out_renderpass->a = a;

  out_renderpass->depth = depth;
  out_renderpass->stencil = stencil;

  // Main subpass
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  // Attachments
  const u32 attachment_description_count = 2;
  VkAttachmentDescription attachment_descriptions[attachment_description_count];

  // Color attachment
  VkAttachmentDescription color_attachment;
  color_attachment.format = context->swapchain.image_format.format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  color_attachment.flags = 0;

  attachment_descriptions[0] = color_attachment;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment =
      0; // attachment description array index
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_reference;

  // Depth attachment, if there is one
  VkAttachmentDescription depth_attachment;
  depth_attachment.format = context->device.depth_format;
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  // depth_attachment.flags = 0;

  attachment_descriptions[1] = depth_attachment;

  VkAttachmentReference depth_attachment_reference;
  depth_attachment_reference.attachment =
      1; // attachment description array index
  depth_attachment_reference.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  subpass.pDepthStencilAttachment = &depth_attachment_reference;

  // TODO: other attachments (input, resolve, preserve)

  // Input from a shader
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = nullptr;

  // Attachments used for multipsampling color attachments
  subpass.pResolveAttachments = nullptr;

  // Attachments not used in this subpass, but must be preserver for the next
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;

  // Renderpass deps
  VkSubpassDependency dependency;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  // Renderpass create
  VkRenderPassCreateInfo renderpass_create_info = {};
  renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderpass_create_info.attachmentCount = attachment_description_count;
  renderpass_create_info.pAttachments = attachment_descriptions;
  renderpass_create_info.subpassCount = 1;
  renderpass_create_info.pSubpasses = &subpass;
  renderpass_create_info.dependencyCount = 1;
  renderpass_create_info.pDependencies = &dependency;
  renderpass_create_info.pNext = nullptr;
  renderpass_create_info.flags = 0;

  VK_CHECK(vkCreateRenderPass(context->device.logical_device,
                              &renderpass_create_info, context->allocator,
                              &out_renderpass->handle));
}

void vulkan_renderpass_destroy(vulkan_context *context,
                               vulkan_renderpass *renderpass) {
  if (renderpass && renderpass->handle) {
    vkDestroyRenderPass(context->device.logical_device, renderpass->handle,
                        context->allocator);
    renderpass->handle = nullptr;
  }
}

void vulkan_renderpass_begin(vulkan_command_buffer *command_buffer,
                             vulkan_renderpass *renderpass,
                             VkFramebuffer frame_buffer) {
  VkRenderPassBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  begin_info.renderPass = renderpass->handle;
  begin_info.framebuffer = frame_buffer;
  begin_info.renderArea.offset.x = renderpass->x;
  begin_info.renderArea.offset.y = renderpass->y;
  begin_info.renderArea.extent.width = renderpass->w;
  begin_info.renderArea.extent.height = renderpass->h;

  VkClearValue clear_values[2];
  lai_zero_memory(clear_values, sizeof(VkClearValue) * 2);
  clear_values[0].color.float32[0] = renderpass->r;
  clear_values[0].color.float32[1] = renderpass->g;
  clear_values[0].color.float32[2] = renderpass->b;
  clear_values[0].color.float32[3] = renderpass->a;
  clear_values[1].depthStencil.depth = renderpass->depth;
  clear_values[1].depthStencil.stencil = renderpass->stencil;

  begin_info.clearValueCount = 2;
  begin_info.pClearValues = clear_values;

  vkCmdBeginRenderPass(command_buffer->handle, &begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
  command_buffer->state = COMMAND_BUFFER_STATE_IN_RENDERPASS;
}

void vulkan_renderpass_end(vulkan_command_buffer *command_buffer,
                           vulkan_renderpass *renderpass) {
  vkCmdEndRenderPass(command_buffer->handle);
  command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}