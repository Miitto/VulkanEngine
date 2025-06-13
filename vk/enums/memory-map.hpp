#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
class MemoryMapFlags {
public:
  enum Flags {
    None = 0,
    Placed = VK_MEMORY_MAP_PLACED_BIT_EXT,
  };

private:
  Flags m_flag;

public:
  MemoryMapFlags(Flags flag = None) : m_flag(flag) {}

  operator VkMemoryMapFlags() const {
    return std::bit_cast<VkMemoryMapFlags>(m_flag);
  }
};
} // namespace vk
