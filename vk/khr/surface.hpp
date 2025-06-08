#pragma once

#include "handle.hpp"
#include "ref.hpp"

#include "khr/surface-capabilities.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class Instance;
class Window;
class PhysicalDevice;

namespace khr {
class Surface : public Handle<VkSurfaceKHR> {
  RawRef<Instance, VkInstance> instance;

public:
  Surface(Instance &instance, VkSurfaceKHR surface);
  auto destroy() -> void override {
    vkDestroySurfaceKHR(instance, m_handle, nullptr);
  }

  Surface(const Surface &) = delete;
  auto operator=(const Surface &) -> Surface & = delete;
  auto operator=(Surface &&o) -> Surface & = delete;

  Surface(Surface &&o) noexcept = default;

  static auto create(Instance &instance, Window &window)
      -> std::optional<Surface>;
};

class SurfaceAttributes {
public:
  vk::khr::SurfaceCapabilities capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  SurfaceAttributes(const PhysicalDevice &device, const Surface &surface);
};
} // namespace khr
} // namespace vk
