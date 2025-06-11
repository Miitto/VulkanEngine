#pragma once

#include "util/vk-logger.hpp"
#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class MemoryProperties {
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
  constexpr MemoryProperties(uint32_t v)
      : value(std::bit_cast<VkMemoryPropertyFlags>(v)) {
#ifndef NDEBUG
    const VkMemoryPropertyFlags validMask =
        DeviceLocal | HostVisible | HostCoherent | HostCached | LazilyAllocated;

    if ((v & ~validMask) != 0) {
      Logger::error(
          fmt::runtime(
              "Invalid MemoryProperties flags: {b}. Valid Mask is: {b}"),
          v, validMask);
    }
#endif
  }
  MemoryProperties(Bits v) : value(v) {}

  constexpr auto operator|(const MemoryProperties &other) const
      -> MemoryProperties {
    return MemoryProperties{value | other.value};
  }

  operator VkMemoryPropertyFlags() const {
    return std::bit_cast<VkMemoryPropertyFlags>(value);
  }
};

} // namespace vk::enums
