#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
class Extent2D;
namespace khr {
class SurfaceCapabilities;
}
} // namespace vk

namespace vk {
class Window {
  GLFWwindow *window = nullptr;

public:
  Window(GLFWwindow *window);
  ~Window();

  Window(const Window &) = delete;
  auto operator=(const Window &) -> Window & = delete;
  Window(Window &&o) noexcept : window(o.window) { o.window = nullptr; }
  auto operator=(Window &&o) noexcept -> Window & {
    window = o.window;
    o.window = nullptr;
    return *this;
  }

  auto operator*() -> GLFWwindow * { return window; }

public:
  static auto create(const char *name, const uint32_t width,
                     const uint32_t height, bool resizable = true)
      -> std::optional<Window>;
  auto shouldClose() -> bool { return glfwWindowShouldClose(window); }

  auto getExtent() -> vk::Extent2D;
  auto getExtent(vk::khr::SurfaceCapabilities &capabilities) -> vk::Extent2D;
};
} // namespace vk
