#pragma once

#include "vk/device/physical.hpp"
#include <functional>

namespace vk::khr {
class Surface;
}
namespace engine {
class PhysicalDeviceSelector {
  std::vector<vk::PhysicalDevice> m_devices;

public:
  PhysicalDeviceSelector(vk::Instance &instance);

  [[nodiscard]] auto hasDevice() const -> bool { return !m_devices.empty(); }

  auto requireExtension(const char *const extension)
      -> PhysicalDeviceSelector &;
  auto requireExtensions(const std::vector<char const *> &extensions)
      -> PhysicalDeviceSelector &;
  auto canPresentTo(const vk::khr::Surface &surface)
      -> PhysicalDeviceSelector &;

  struct DeviceRating {
    vk::PhysicalDevice device;
    uint32_t rating;
  };

  auto rateDevices(
      const std::function<uint32_t(vk::PhysicalDevice &device)> &predicate)
      -> std::vector<DeviceRating>;
};
} // namespace engine
