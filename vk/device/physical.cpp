#include "physical.hpp"

#include "queue.hpp"

#include <cstring>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
auto PhysicalDevice::all(Instance &instance) -> std::vector<PhysicalDevice> {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    return {};
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

  std::vector<PhysicalDevice> physicalDevices;
  physicalDevices.reserve(devices.size());

  for (auto device : devices) {
    physicalDevices.emplace_back(device);
  }
  return physicalDevices;
}

auto PhysicalDevice::getProperties() const -> PhysicalDeviceProperties {
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(m_handle, &props);
  return props;
}

auto PhysicalDevice::getFeatures() const -> PhysicalDeviceFeatures {
  VkPhysicalDeviceFeatures feats;
  vkGetPhysicalDeviceFeatures(m_handle, &feats);
  return feats;
}

auto PhysicalDevice::getExtensions() const -> std::vector<ExtensionProperties> {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extensionCount,
                                       nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extensionCount,
                                       extensions.data());

  std::vector<ExtensionProperties> extProps;
  extProps.reserve(extensionCount);

  for (const auto &ext : extensions) {
    extProps.emplace_back(ext);
  }

  return extProps;
}

auto PhysicalDevice::getMemoryProperties() const
    -> VkPhysicalDeviceMemoryProperties {
  VkPhysicalDeviceMemoryProperties props;
  vkGetPhysicalDeviceMemoryProperties(m_handle, &props);
  return props;
}

auto PhysicalDevice::getQueues() -> std::vector<QueueFamily> {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount,
                                           families.data());

  std::vector<QueueFamily> queueFamilies;
  queueFamilies.reserve(queueFamilyCount);

  for (size_t i = 0; i < queueFamilyCount; ++i) {
    queueFamilies.emplace_back(*this, families[i], static_cast<uint32_t>(i));
  }

  return queueFamilies;
}

auto PhysicalDevice::findUnsupportedExtensions(
    const std::vector<char const *> &extensions) const
    -> std::vector<char const *> {
  std::vector<ExtensionProperties> availableExtensions = getExtensions();
  std::vector<char const *> unsupportedExtensions;

  for (auto requested : extensions) {
    bool found = false;
    for (const auto &extension : availableExtensions) {
      if (strcmp(requested, extension.extensionName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      unsupportedExtensions.push_back(requested);
    }
  }

  return unsupportedExtensions;
}

auto PhysicalDevice::findMemoryType(uint32_t typeFilter,
                                    enums::MemoryPropertyFlags properties) const
    -> std::optional<DeviceMemoryProperties> {
  VkPhysicalDeviceMemoryProperties memProperties = getMemoryProperties();

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return DeviceMemoryProperties{.index = i,
                                    .memType = memProperties.memoryTypes[i]};
    }
  }

  return std::nullopt;
}
} // namespace vk
