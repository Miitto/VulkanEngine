#pragma once

#include "offset.hpp"
#include "structs/extent2d.hpp"
#include <vulkan/vulkan_core.h>

namespace vk {
struct Rect2D : public VkRect2D {
  Rect2D(uint32_t width, uint32_t height, int32_t x = 0, int32_t y = 0)
      : VkRect2D{.offset = {x, y}, .extent = {width, height}} {}
  Rect2D(uint32_t width, uint32_t height, Offset2D offset)
      : VkRect2D{.offset = offset, .extent = {width, height}} {}

  Rect2D(Extent2D extent, Offset2D offset = {0, 0})
      : VkRect2D{.offset = offset, .extent = extent} {}

  Rect2D(const VkRect2D &rect) : VkRect2D(rect) {}
};
} // namespace vk
