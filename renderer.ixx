module;
#include <GLFW/glfw3.h>
export module renderer;

export import vk;

export namespace renderer {
bool init() {
  if (!glfwInit()) {
    return false;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  return true;
}
void pollEvents() { glfwPollEvents(); }

void terminate() { glfwTerminate(); }
} // namespace renderer
