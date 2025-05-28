module;

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>
module vk;

import util;
import :info.framebufferCreate;

namespace vk {
std::optional<Framebuffer>
Framebuffer::create(Device &device, vk::info::FramebufferCreate info) {

  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(*device, &info, nullptr, &framebuffer) !=
      VK_SUCCESS) {
    util::log_err("Failed to create framebuffer");
    return std::nullopt;
  }

  return Framebuffer(framebuffer, device);
}
} // namespace vk
