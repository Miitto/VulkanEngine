#include "instance.hpp"

#include "util/vk-logger.hpp"

#include "khr/surface.hpp"

#include <optional>
#include <sstream>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {

namespace info {
auto InstanceCreate::setupLayers() -> void {
  auto check = checkLayers();

  while (check.has_value()) {
    for (size_t i = 0; i < layers.size(); i++) {
      if (strcmp(layers[i], *check) == 0) {
        layers.erase(layers.begin() + static_cast<long long>(i));
        break;
      }
    }

    check = checkLayers();
  }

  enabledLayerCount = static_cast<uint32_t>(layers.size());
  ppEnabledLayerNames = layers.data();
}
auto InstanceCreate::checkValidationLayerSupport()
    -> std::optional<const char *> {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  if (layerCount == 0) {
    return std::nullopt;
  }

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : layers) {
    bool layerFound = false;
    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      Logger::error("Validation layer {} not found.", layerName);
      return layerName;
    }
  }
  return std::nullopt;
}

auto InstanceCreate::enableGLFWExtensions() -> InstanceCreate & {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<char *> glfwExt(glfwExtensionCount);
  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    glfwExt[i] = const_cast<char *>(glfwExtensions[i]);
  }
  std::stringstream log{};
  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    log << glfwExtensions[i];
    if (i != glfwExtensionCount - 1) {
      log << ", ";
    }
  }
  log << ')';
  Logger::info("Enabling {} GLFW extensions ({})", glfwExtensionCount,
               log.str());
  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    extensions.push_back(glfwExtensions[i]);
  }

  setupExtensions();

  return *this;
}
} // namespace info

auto Instance::create(info::InstanceCreate &createInfo)
    -> std::optional<Instance> {
  VkInstance instance;
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    return std::nullopt;
  }
  return Instance(instance);
}

auto Instance::createSurface(Window &window) -> std::optional<khr::Surface> {
  return khr::Surface::create(*this, window);
}
} // namespace vk
