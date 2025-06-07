#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class MemoryMapFlags {
public:
  enum Bits {
    Placed = VK_MEMORY_MAP_PLACED_BIT_EXT,
  };

private:
  VkMemoryMapFlags value;

public:
  MemoryMapFlags(VkMemoryMapFlags v)
      : value(std::bit_cast<VkMemoryMapFlags>(v)) {}
  MemoryMapFlags(Bits v) : value(v) {}

  auto operator|(MemoryMapFlags &other) const -> MemoryMapFlags {
    return {value | other.value};
  }
  operator VkMemoryMapFlags() const {
    return std::bit_cast<VkMemoryMapFlags>(value);
  }
};
} // namespace vk::enums
