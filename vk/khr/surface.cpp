#include "surface.hpp"

#include "device/physical.hpp"
#include "instance.hpp"

#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk::khr {
Surface::Surface(Instance &instance, VkSurfaceKHR surface)
    : Handle<VkSurfaceKHR>(surface), instance(instance.ref()) {}

auto Surface::create(Instance &instance, Window &window)
    -> std::optional<Surface> {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(*instance, *window, nullptr, &surface) !=
      VK_SUCCESS) {
    return std::nullopt;
  }
  return Surface(instance, surface);
}

SurfaceAttributes::SurfaceAttributes(const PhysicalDevice &physicalDevice,
                                     const Surface &surface) {

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, *surface,
                                            &capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface, &formatCount,
                                       nullptr);
  if (formatCount != 0) {
    formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface,
                                         &formatCount, formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface,
                                            &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        *physicalDevice, *surface, &presentModeCount, presentModes.data());
  }
}
} // namespace vk::khr
