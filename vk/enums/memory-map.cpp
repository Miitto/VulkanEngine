#include "memory-map.hpp"

#include "util/vk-logger.hpp"
#include <vulkan/vulkan_core.h>

namespace vk::enums {
MemoryMap::MemoryMap(const VkMemoryMapFlags v)
    : value(std::bit_cast<VkMemoryMapFlags>(v)) {
#ifndef NDEBUG
  const VkMemoryMapFlags validMask =
      VK_MEMORY_MAP_PLACED_BIT_EXT; // Currently only Placed is valid
  if (v & ~validMask) {
    Logger::error("Invalid MemoryMap flags: #{}. Valid Mask is: #{}", v,
                  validMask);
  }
#endif
}
} // namespace vk::enums
