#include "window.hpp"

#include "khr/surface-capabilities.hpp"
#include "structs/extent2d.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <limits>
#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
Window::Window(GLFWwindow *window) : window(window) {}

Window::~Window() {
  if (window != nullptr) {
    glfwDestroyWindow(window);
  }
}

auto Window::create(const char *name, const uint32_t width,
                    const uint32_t height, bool resizable)
    -> std::optional<Window> {
  if (!resizable) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  } else {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }

  auto window =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name,
                       nullptr, nullptr);

  if (window == nullptr) {
    return std::nullopt;
  }

  return Window(window);
}

auto Window::getExtent() -> Extent2D {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  return Extent2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

auto Window::getExtent(khr::SurfaceCapabilities &capabilities) -> Extent2D {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  Extent2D actualExtent(static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height));

  actualExtent.width =
      std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);
  actualExtent.height =
      std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  return actualExtent;
}
} // namespace vk
