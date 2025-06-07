#include "fence.hpp"

#include "device/device.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
Fence::Fence(VkFence fence, Device &device)
    : Handle(fence), m_device(device.ref()) {}

auto Fence::create(Device &device, bool makeSignalled) -> std::optional<Fence> {
  vk::info::FenceCreate createInfo(makeSignalled);

  return create(device, createInfo);
}

auto Fence::create(Device &device, vk::info::FenceCreate createInfo)
    -> std::optional<Fence> {
  VkFence fence;

  auto result = vkCreateFence(device, &createInfo, nullptr, &fence);
  if (result != VK_SUCCESS) {
    return std::nullopt;
  }

  Fence f(fence, device);

  return f;
}

auto Fence::wait(bool autoReset, uint64_t timeout) -> void {
  VkResult result = vkWaitForFences(m_device, 1, &m_handle, VK_TRUE, timeout);
  if (result != VK_SUCCESS && result != VK_TIMEOUT) {
    throw std::runtime_error("Failed to wait for fence");
  }

  if (autoReset) {
    reset();
  }
}

auto Fence::reset() -> void { vkResetFences(m_device, 1, &m_handle); }

auto Fence::destroy() -> void { vkDestroyFence(m_device, m_handle, nullptr); }
} // namespace vk
