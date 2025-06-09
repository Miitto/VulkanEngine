#pragma once

#include "handle.hpp"
#include "ref.hpp"
#include "version.hpp"
#include "window.hpp"
#include <optional>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
namespace khr {
class Surface;
}

namespace info {
class Application : public VkApplicationInfo {
public:
  Application(const char *name = "Vulkan App",
              Version version = {.major = 1, .minor = 0, .patch = 0},
              const char *engineName = "No Engine",
              Version engineVersion = {.major = 1, .minor = 0, .patch = 0}) {
    pApplicationName = name;
    sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    pNext = nullptr;
    applicationVersion = version;
    pEngineName = engineName;
    this->engineVersion = engineVersion;
    apiVersion = VK_API_VERSION_1_0;
  }
};

class InstanceCreate : public VkInstanceCreateInfo {
  std::vector<const char *> layers;
  std::vector<const char *> extensions;

  auto checkValidationLayerSupport() -> std::optional<const char *>;
  auto checkLayers() -> std::optional<const char *> {
    if (layers.empty()) {
      return std::nullopt;
    }

    return checkValidationLayerSupport();
  }

  void setupExtensions() {
    enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    ppEnabledExtensionNames = extensions.data();
  }

  void setupLayers();

public:
  enum class ValidationType : uint8_t { NONE, DEBUG, ALWAYS };
  InstanceCreate(info::Application &appInfo,
                 ValidationType validation = ValidationType::DEBUG,
                 bool glfwExtensions = true)
      : VkInstanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = nullptr,
        } {

    switch (validation) {
    case ValidationType::DEBUG: {
#ifndef NDEBUG
      enableValidationLayers();
#endif
      break;
    }
    case ValidationType::ALWAYS: {
      enableValidationLayers();
      break;
    }
    default: {
      break;
    }
    }

    if (glfwExtensions) {
      enableGLFWExtensions();
    }
  }

  auto addLayer(const char *layer) -> InstanceCreate & {
    layers.push_back(layer);

    setupLayers();
    return *this;
  }

  auto addExtension(const char *extension) -> InstanceCreate & {
    extensions.push_back(extension);

    setupExtensions();
    return *this;
  }

  auto enableGLFWExtensions() -> InstanceCreate &;

  auto enableValidationLayers() -> InstanceCreate & {
    layers.push_back("VK_LAYER_KHRONOS_validation");

    setupLayers();

    return *this;
  }

  InstanceCreate(const InstanceCreate &other)
      : VkInstanceCreateInfo{other}, layers(other.layers),
        extensions(other.extensions) {
    setupLayers();
    setupExtensions();
  }

  InstanceCreate(InstanceCreate &&other) noexcept
      : VkInstanceCreateInfo{other}, layers(std::move(other.layers)),
        extensions(std::move(other.extensions)) {
    setupLayers();
    setupExtensions();
    other.setupLayers();
    other.setupExtensions();
  }
};

} // namespace info

class Instance : public RawRefable<Instance, VkInstance>,
                 public Handle<VkInstance> {

public:
  Instance(VkInstance instance) : RawRefable(), Handle(instance) {}
  auto destroy() -> void override { vkDestroyInstance(m_handle, nullptr); }

  Instance(Instance &&o) noexcept = default;

  static auto create(info::InstanceCreate &createInfo)
      -> std::optional<Instance>;

  auto createSurface(Window &window) -> std::optional<khr::Surface>;
};
} // namespace vk
