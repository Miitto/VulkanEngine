#pragma once

#include "util/vk-logger.hpp"
#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
enum class MemoryProperties {
  DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
  HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
  LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
};

} // namespace vk
