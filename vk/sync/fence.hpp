#pragma once

#include "handle.hpp"
#include "ref.hpp"
#include "vulkan/vulkan_core.h"
#include <optional>

namespace vk {
class Device;

namespace info {
class FenceCreate : public VkFenceCreateInfo {
public:
  FenceCreate(bool signaled = false)
      : VkFenceCreateInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                          .pNext = nullptr,
                          .flags = signaled ? static_cast<uint32_t>(
                                                  VK_FENCE_CREATE_SIGNALED_BIT)
                                            : 0u} {}
};
} // namespace info

class Fence : public Handle<VkFence> {
  RawRef<Device, VkDevice> m_device;

public:
  Fence(VkFence semaphore, Device &device);

  Fence(Fence &&other) noexcept = default;

  static auto create(Device &device, bool makeSignalled = false)
      -> std::optional<Fence>;
  static auto create(Device &device, vk::info::FenceCreate info)
      -> std::optional<Fence>;

  void wait(bool autoReset = false, uint64_t timeout = UINT64_MAX);

  void reset();

  auto destroy() -> void override;
};
} // namespace vk
