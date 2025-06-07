#include "handle.hpp"
#include "ref.hpp"
#include "vulkan/vulkan_core.h"
#include <optional>
#include <vector>

namespace vk {
class Device;
class RenderPass;
namespace info {
class FramebufferCreate;
}
} // namespace vk

namespace vk {
namespace info {
class FramebufferCreate : public VkFramebufferCreateInfo {
  std::vector<VkImageView> m_attachments;

  void setupAttachments() {
    attachmentCount = static_cast<uint32_t>(m_attachments.size());
    pAttachments = m_attachments.empty() ? nullptr : m_attachments.data();
  }

public:
  FramebufferCreate(RenderPass &renderPass, VkExtent2D extent)
      : FramebufferCreate(renderPass, extent.width, extent.height) {}
  FramebufferCreate(RenderPass &renderPass, uint32_t width, uint32_t height);

  auto addAttachment(VkImageView attachment) -> FramebufferCreate & {
    m_attachments.push_back(attachment);
    setupAttachments();
    return *this;
  }

  auto addAttachments(std::vector<VkImageView> attchmnts)
      -> FramebufferCreate & {
    m_attachments.insert(m_attachments.end(), attchmnts.begin(),
                         attchmnts.end());
    setupAttachments();
    return *this;
  }

  auto setAttachments(std::vector<VkImageView> attachments)
      -> FramebufferCreate & {
    this->m_attachments = std::move(attachments);
    setupAttachments();
    return *this;
  }

  FramebufferCreate(const FramebufferCreate &other)
      : VkFramebufferCreateInfo{other}, m_attachments(other.m_attachments) {
    setupAttachments();
  }

  FramebufferCreate(FramebufferCreate &&other) noexcept
      : VkFramebufferCreateInfo{other},
        m_attachments(std::move(other.m_attachments)) {
    setupAttachments();
    other.setupAttachments();
  }
};

} // namespace info
class Framebuffer : public Handle<VkFramebuffer> {
  RawRef<Device, VkDevice> device;

public:
  Framebuffer(VkFramebuffer framebuffer, Device &device);
  Framebuffer(Framebuffer &&other) noexcept = default;

  static auto create(Device &device, info::FramebufferCreate info)
      -> std::optional<Framebuffer>;
  auto destroy() -> void override;
};
} // namespace vk
