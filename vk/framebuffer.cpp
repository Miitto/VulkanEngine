#include "framebuffer.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"
#include "pipeline/renderPass.hpp"

#include <cstdint>
#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
namespace info {
FramebufferCreate::FramebufferCreate(RenderPass &renderPass, uint32_t width,
                                     uint32_t height)
    : VkFramebufferCreateInfo{.sType =
                                  VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                              .pNext = nullptr,
                              .flags = 0,
                              .renderPass = renderPass,
                              .attachmentCount = 0,
                              .pAttachments = nullptr,
                              .width = width,
                              .height = height,
                              .layers = 1} {}
} // namespace info
Framebuffer::Framebuffer(VkFramebuffer framebuffer, Device &device)
    : Handle(framebuffer), device(device.ref()) {}

auto Framebuffer::destroy() -> void {
  vkDestroyFramebuffer(device, m_handle, nullptr);
}

auto Framebuffer::create(Device &device, info::FramebufferCreate info)
    -> std::optional<Framebuffer> {

  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(*device, &info, nullptr, &framebuffer) !=
      VK_SUCCESS) {
    Logger::error("Failed to create framebuffer");
    return std::nullopt;
  }

  return Framebuffer(framebuffer, device);
}
} // namespace vk
