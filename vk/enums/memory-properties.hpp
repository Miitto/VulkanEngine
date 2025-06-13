#pragma once

#include "util/vk-logger.hpp"
#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
class MemoryProperties {
public:
  enum Bits : VkMemoryPropertyFlags {
    None = 0,
    DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
  };

private:
  VkMemoryPropertyFlags m_flags;

  static constexpr VkMemoryPropertyFlags allFlags =
      DeviceLocal | HostVisible | HostCoherent | HostCached | LazilyAllocated;

public:
  constexpr MemoryProperties() : m_flags(None) {}
  constexpr MemoryProperties(Bits bit) : m_flags(bit) {}
  constexpr MemoryProperties(const VkMemoryPropertyFlags flags)
      : m_flags(flags) {
    if (flags & ~allFlags) {
      if (std::is_constant_evaluated()) {
        throw std::invalid_argument("Invalid VkBufferUsageFlags provided");
      } else {
        Logger::error(
            "Invalid VkBufferUsageFlags provided: {:b}. Valid Mask: {:b}",
            flags, allFlags);
      }
    }
  }

  constexpr operator VkMemoryPropertyFlags() const { return m_flags; }
  constexpr auto operator|(Bits bit) const -> MemoryProperties {
    return (m_flags | bit);
  }
  constexpr auto operator|(const MemoryProperties &other) const
      -> MemoryProperties {
    return (m_flags | other.m_flags);
  }
};
} // namespace vk
