#pragma once

#include "handle.hpp"
#include <vulkan/vulkan_core.h>

namespace vk {
class Image : public Handle<VkImage> {

  Image(VkImage handle) : Handle(handle) {}

public:
  static auto fromHandle(VkImage handle) -> Image { return Image{handle}; }
};
} // namespace vk
