#include "semaphore.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

#include "device/device.hpp"

namespace vk {

Semaphore::Semaphore(VkSemaphore semaphore, Device &device)
    : Handle(semaphore), device(device.ref()) {}
auto Semaphore::create(Device &device) -> std::optional<Semaphore> {
  info::SemaphoreCreate createInfo{};

  return create(device, createInfo);
}

auto Semaphore::create(Device &device, info::SemaphoreCreate createInfo)
    -> std::optional<Semaphore> {
  VkSemaphore semaphore;

  auto result = vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
  if (result != VK_SUCCESS) {
    return std::nullopt;
  }

  Semaphore sem(semaphore, device);

  return sem;
}
} // namespace vk
