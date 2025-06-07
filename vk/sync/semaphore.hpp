#pragma once

#include "handle.hpp"
#include "ref.hpp"
#include "vulkan/vulkan_core.h"
#include <optional>

namespace vk {
class Device;
}

namespace vk {

namespace info {
class SemaphoreCreate : public VkSemaphoreCreateInfo {

public:
  SemaphoreCreate()
      : VkSemaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                              .pNext = nullptr,
                              .flags = 0} {}
};

} // namespace info

class Semaphore : public Handle<VkSemaphore> {
  RawRef<Device, VkDevice> device;

public:
  Semaphore(VkSemaphore semaphore, Device &device);

  Semaphore(Semaphore &&other) noexcept = default;

  static auto create(Device &device) -> std::optional<Semaphore>;
  static auto create(Device &device, info::SemaphoreCreate info)
      -> std::optional<Semaphore>;

  auto destroy() -> void override {
    vkDestroySemaphore(device, m_handle, nullptr);
  }
};
} // namespace vk
