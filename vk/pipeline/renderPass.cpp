#include "renderPass.hpp"

#include "device/device.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
RenderPass::RenderPass(Device &device, VkRenderPass handle)
    : Handle<VkRenderPass>(handle), device(device.ref()) {}

auto RenderPass::create(Device &device, vk::info::RenderPassCreate info)
    -> std::optional<RenderPass> {

  VkRenderPass renderPass;
  if (vkCreateRenderPass(device, &info, nullptr, &renderPass) != VK_SUCCESS) {
    return std::nullopt;
  }

  RenderPass pass(device.ref(), renderPass);

  return pass;
}

auto RenderPass::destroy() -> void {
  vkDestroyRenderPass(**device, m_handle, nullptr);
}
} // namespace vk
