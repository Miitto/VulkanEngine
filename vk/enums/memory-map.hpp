#pragma once

#include <vulkan/vulkan_core.h>

namespace vk {
enum class MemoryMapFlags {
  None = 0,
  Placed = VK_MEMORY_MAP_PLACED_BIT_EXT,
};
} // namespace vk
