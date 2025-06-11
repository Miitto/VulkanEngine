#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class MemoryMap {
public:
  enum Bits {
    None = 0,
    Placed = VK_MEMORY_MAP_PLACED_BIT_EXT,
  };

private:
  VkMemoryMapFlags value;

public:
  explicit MemoryMap(VkMemoryMapFlags v);
  MemoryMap(Bits v) : value(v) {}

  auto operator|(MemoryMap &other) const -> MemoryMap {
    return MemoryMap{value | other.value};
  }
  operator VkMemoryMapFlags() const {
    return std::bit_cast<VkMemoryMapFlags>(value);
  }
};
} // namespace vk::enums
