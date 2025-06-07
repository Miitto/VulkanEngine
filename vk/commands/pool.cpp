#include "pool.hpp"

#include "util/vk-logger.hpp"

#include "commands/buffer.hpp"
#include "device/device.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
CommandPool::CommandPool(VkCommandPool commandPool, Device &device)
    : Handle(commandPool), device(device.ref()) {}
auto CommandPool::create(Device &device, vk::info::CommandPoolCreate info)
    -> std::optional<CommandPool> {
  VkCommandPool commandPool;
  auto result = vkCreateCommandPool(device, &info, nullptr, &commandPool);
  if (result != VK_SUCCESS) {
    return std::nullopt;
  }

  return CommandPool(commandPool, device);
}

auto CommandPool::allocBuffer(bool secondary) const
    -> std::optional<CommandBuffer> {
  VkCommandBuffer commandBuffer;

  info::CommandBufferAllocate bufferInfo(*this, 1, secondary);

  if (vkAllocateCommandBuffers(device, &bufferInfo, &commandBuffer) !=
      VK_SUCCESS) {
    Logger::error("Failed to allocate command buffer");
    return std::nullopt;
  }

  return commandBuffer;
}

auto CommandPool::allocBuffers(uint32_t count, bool secondary) const
    -> std::optional<std::vector<CommandBuffer>> {
  std::vector<VkCommandBuffer> rawCommandBuffers;
  rawCommandBuffers.resize(count);

  info::CommandBufferAllocate bufferInfo(*this, count, secondary);

  if (vkAllocateCommandBuffers(device.raw(), &bufferInfo,
                               rawCommandBuffers.data()) != VK_SUCCESS) {
    Logger::error("Failed to allocate command buffers");
    return std::nullopt;
  }

  std::vector<CommandBuffer> commandBuffers;

  for (auto &rawCommandBuffer : rawCommandBuffers) {
    if (rawCommandBuffer == VK_NULL_HANDLE) {
      Logger::error("Failed to allocate command buffer, got VK_NULL_HANDLE");
      return std::nullopt;
    }

    commandBuffers.emplace_back(rawCommandBuffer);
  }

  if (commandBuffers.size() != count) {
    Logger::error(
        "Failed to allocate all command buffers, allocated {} buffers "
        "instead of {}",
        commandBuffers.size(), count);
    return std::nullopt;
  }

  return commandBuffers;
}
} // namespace vk
