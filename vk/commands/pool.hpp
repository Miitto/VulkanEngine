#pragma once

#include "handle.hpp"
#include "queue.hpp"
#include "ref.hpp"

#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace vk {
class Device;
class CommandBuffer;

namespace info {
class CommandBufferAllocate;

class CommandPoolCreate : public VkCommandPoolCreateInfo {

public:
  CommandPoolCreate(QueueFamily &queueFamily, bool resetable = false,
                    bool transient = false)
      : CommandPoolCreate(queueFamily.getIndex(), resetable, transient) {}
  CommandPoolCreate(uint32_t queueFamily, bool resetable = false,
                    bool transient = false)
      : VkCommandPoolCreateInfo{.sType =
                                    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                .pNext = nullptr,
                                .flags = 0,
                                .queueFamilyIndex = queueFamily} {

    if (resetable) {
      flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }

    if (transient) {
      flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }
  }

  auto setFlags(VkCommandPoolCreateFlags f) -> CommandPoolCreate & {
    flags = f;
    return *this;
  }

  auto setQueueFamilyIndex(uint32_t index) -> CommandPoolCreate & {
    queueFamilyIndex = index;
    return *this;
  }

  auto setPNext(void *next) -> CommandPoolCreate & {
    pNext = next;
    return *this;
  }

  auto resetable() -> CommandPoolCreate & {
    flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    return *this;
  }

  auto transient() -> CommandPoolCreate & {
    flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    return *this;
  }
};

} // namespace info

class CommandPool : public Handle<VkCommandPool> {
  RawRef<Device, VkDevice> device;

public:
  CommandPool(CommandPool &&other) = default;

  CommandPool(VkCommandPool commandPool, Device &device);

  [[nodiscard]] auto getDevice() const -> const RawRef<Device, VkDevice> & {
    return device;
  }

  static auto create(Device &device, info::CommandPoolCreate info)
      -> std::optional<CommandPool>;

  [[nodiscard]] auto allocBuffer(bool secondary = false) const
      -> std::optional<CommandBuffer>;
  [[nodiscard]] auto allocBuffers(uint32_t count, bool secondary = false) const
      -> std::optional<std::vector<CommandBuffer>>;
  auto allocBuffers(info::CommandBufferAllocate &info) const
      -> std::optional<std::vector<CommandBuffer>>;
};

namespace info {
class CommandBufferAllocate : public VkCommandBufferAllocateInfo {

public:
  CommandBufferAllocate(const CommandPool &commandPool, uint32_t count = 1,
                        bool secondary = false)
      : VkCommandBufferAllocateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY
                               : VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count} {}
};

} // namespace info
} // namespace vk
