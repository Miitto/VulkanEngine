#pragma once

#include <vulkan/vulkan_core.h>

namespace vk {
class Extent2D : public VkExtent2D {
public:
  Extent2D(uint32_t w = 0, uint32_t h = 0)
      : VkExtent2D{.width = w, .height = h} {}

  Extent2D(const VkExtent2D &extent) : VkExtent2D(extent) {}
};
} // namespace vk
