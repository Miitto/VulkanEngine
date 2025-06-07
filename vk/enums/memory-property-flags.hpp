#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class MemoryPropertyFlags {
public:
  enum Bits {
    DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
  };

private:
  VkMemoryPropertyFlags value;

public:
  MemoryPropertyFlags(uint32_t v)
      : value(std::bit_cast<VkMemoryPropertyFlags>(v)) {}
  MemoryPropertyFlags(Bits v) : value(v) {}

  auto operator|(MemoryPropertyFlags &other) const -> MemoryPropertyFlags {
    return {value | other.value};
  }

  operator VkMemoryPropertyFlags() const {
    return std::bit_cast<VkMemoryPropertyFlags>(value);
  }
};

} // namespace vk::enums
