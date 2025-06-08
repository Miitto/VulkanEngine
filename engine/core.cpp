#include "core.hpp"

#include "util/vk-logger.hpp"

namespace engine {
auto init() -> bool {
  vk::Logger::init();

  if (!glfwInit()) {
    return false;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  return true;
}
void pollEvents() { glfwPollEvents(); }

void terminate() { glfwTerminate(); }
} // namespace engine
