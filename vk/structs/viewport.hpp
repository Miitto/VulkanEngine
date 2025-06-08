#pragma once
#include <vulkan/vulkan_core.h>

#include "khr/swapchain.hpp"

namespace vk {
class Viewport : public VkViewport {
public:
  Viewport(float width, float height)
      : VkViewport{.x = 0.0f,
                   .y = 0.0f,
                   .width = width,
                   .height = height,
                   .minDepth = 0.0f,
                   .maxDepth = 1.0f} {}

  Viewport(const VkViewport &viewport) : VkViewport(viewport) {}

  Viewport(Extent2D extent)
      : Viewport(static_cast<float>(extent.width),
                 static_cast<float>(extent.height)) {}

  Viewport(khr::Swapchain &swapChain) : Viewport(swapChain.getExtent()) {}
};
} // namespace vk
