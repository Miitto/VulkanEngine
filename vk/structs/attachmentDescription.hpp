#pragma once

#include <vulkan/vulkan_core.h>

#include "enums/format.hpp"

namespace vk {

class AttachmentDescription : public VkAttachmentDescription {
public:
  AttachmentDescription(Format &format)
      : VkAttachmentDescription{
            .flags = 0,
            .format = format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_UNDEFINED} {}

  auto setColorDepth(VkAttachmentLoadOp colorDepthLoadOp,
                     VkAttachmentStoreOp colorDepthStoreOp)
      -> AttachmentDescription & {
    loadOp = colorDepthLoadOp;
    storeOp = colorDepthStoreOp;
    return *this;
  }

  auto setStencil(VkAttachmentLoadOp load, VkAttachmentStoreOp store)
      -> AttachmentDescription & {
    stencilLoadOp = load;
    stencilStoreOp = store;
    return *this;
  }

  auto setInitialLayout(VkImageLayout layout) -> AttachmentDescription & {
    initialLayout = layout;
    return *this;
  }

  auto setFinalLayout(VkImageLayout layout) -> AttachmentDescription & {
    finalLayout = layout;
    return *this;
  }
};
} // namespace vk
