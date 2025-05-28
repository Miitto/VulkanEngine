module;

#include <vulkan/vulkan_core.h>

export module vk:rect2d;

namespace vk {
export struct Rect2D : public VkRect2D {
  Rect2D(uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0)
      : VkRect2D{.offset = {0, 0}, .extent = {width, height}} {}
  Rect2D(uint32_t width, uint32_t height, VkOffset2D offset)
      : VkRect2D{.offset = offset, .extent = {width, height}} {}

  Rect2D(VkExtent2D extent, VkOffset2D offset = {0, 0})
      : VkRect2D{.offset = offset, .extent = extent} {}

  Rect2D(const VkRect2D &rect) : VkRect2D(rect) {}
};
} // namespace vk
