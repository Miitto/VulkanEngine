#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

namespace vk {
class SubpassDescription : public VkSubpassDescription {
  std::vector<VkAttachmentReference> colorReferences;
  std::vector<VkAttachmentReference> inputReferences;
  std::vector<VkAttachmentReference> resolveReferences;
  VkAttachmentReference depthReference;
  std::vector<uint32_t> preserveReferences;

  void setupInputAttachments() {
    inputAttachmentCount = static_cast<uint32_t>(inputReferences.size());
    pInputAttachments = inputReferences.data();
  }

  void setupColorAttachments() {
    colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
    pColorAttachments = colorReferences.data();
  }

  void setupResolveAttachments() {
    pResolveAttachments = resolveReferences.data();
  }

  void setupPreserveAttachments() {
    preserveAttachmentCount = static_cast<uint32_t>(preserveReferences.size());
    pPreserveAttachments = preserveReferences.data();
  }

public:
  SubpassDescription()
      : VkSubpassDescription{.flags = 0,
                             .pipelineBindPoint =
                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                             .inputAttachmentCount = 0,
                             .pInputAttachments = nullptr,
                             .colorAttachmentCount = 0,
                             .pColorAttachments = nullptr,
                             .pResolveAttachments = nullptr,
                             .pDepthStencilAttachment = nullptr,
                             .preserveAttachmentCount = 0,
                             .pPreserveAttachments = nullptr} {}

  auto input(VkAttachmentReference attachment) -> SubpassDescription & {
    inputReferences.push_back(attachment);
    setupInputAttachments();
    return *this;
  }

  auto color(VkAttachmentReference attachment) -> SubpassDescription & {
    colorReferences.push_back(attachment);
    setupColorAttachments();
    return *this;
  }

  auto resolve(VkAttachmentReference attachment) -> SubpassDescription & {
    resolveReferences.push_back(attachment);
    setupResolveAttachments();
    return *this;
  }

  auto depthStencil(VkAttachmentReference &attachment) -> SubpassDescription & {
    depthReference = attachment;
    pDepthStencilAttachment = &depthReference;
    return *this;
  }

  auto preserve(uint32_t attachment) -> SubpassDescription & {
    preserveReferences.push_back(attachment);
    setupPreserveAttachments();
    return *this;
  }
};
} // namespace vk
