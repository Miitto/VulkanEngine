#pragma once

#include "enums/memory-properties.hpp"
#include "instance.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class QueueFamilies;

struct DeviceMemoryProperties {
  uint32_t index;
  VkMemoryType memType;
};

class PhysicalDeviceProperties : public VkPhysicalDeviceProperties {
public:
  PhysicalDeviceProperties() = default;
  PhysicalDeviceProperties(const VkPhysicalDeviceProperties &props)
      : VkPhysicalDeviceProperties(props) {}
};

class PhysicalDeviceFeatures : public VkPhysicalDeviceFeatures {
public:
  PhysicalDeviceFeatures() = default;
  PhysicalDeviceFeatures(const VkPhysicalDeviceFeatures &features)
      : VkPhysicalDeviceFeatures(features) {}
};

class ExtensionProperties : public VkExtensionProperties {
public:
  ExtensionProperties() = default;
  ExtensionProperties(const VkExtensionProperties &props)
      : VkExtensionProperties(props) {}

  [[nodiscard]] auto getName() const -> const char * { return extensionName; }
};

class PhysicalDevice : public Handle<VkPhysicalDevice> {

public:
  PhysicalDevice(VkPhysicalDevice device) : Handle(device) {}
  PhysicalDevice(const PhysicalDevice &other) : Handle(other.m_handle) {}
  auto operator=(const PhysicalDevice &other) -> PhysicalDevice & {
    if (this != &other) {
      m_handle = other.m_handle;
    }
    return *this;
  }
  PhysicalDevice(PhysicalDevice &&other) noexcept = default;

  static auto all(Instance &instance) -> std::vector<PhysicalDevice>;

  [[nodiscard]] auto getProperties() const -> PhysicalDeviceProperties;
  [[nodiscard]] auto getFeatures() const -> PhysicalDeviceFeatures;
  [[nodiscard]] auto getExtensions() const -> std::vector<ExtensionProperties>;
  [[nodiscard]] auto getQueues() const -> QueueFamilies;

  [[nodiscard]] auto getMemoryProperties() const
      -> VkPhysicalDeviceMemoryProperties;

  [[nodiscard]] auto supportsExtension(const char *const extension) const
      -> bool;

  [[nodiscard]] auto
  supportsExtensions(const std::vector<char const *> &extensions) const -> bool;

  [[nodiscard]] auto findMemoryType(uint32_t typeFilter,
                                    enums::MemoryProperties properties) const
      -> std::optional<DeviceMemoryProperties>;

  auto isDiscrete() -> bool {
    return getProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
  }
};
} // namespace vk
