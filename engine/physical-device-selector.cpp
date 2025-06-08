#include "physical-device-selector.hpp"

#include <algorithm>

#include "khr/surface.hpp"
#include "logger.hpp"

namespace engine {

PhysicalDeviceSelector::PhysicalDeviceSelector(vk::Instance &instance) {
  m_devices = vk::PhysicalDevice::all(instance);
  Logger::info("Found {} physical devices", m_devices.size());
}

auto PhysicalDeviceSelector::requireExtension(const char *const extension)
    -> PhysicalDeviceSelector & {
  return requireExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME});
}

auto PhysicalDeviceSelector::requireExtensions(
    const std::vector<char const *> &extensions) -> PhysicalDeviceSelector & {
  std::erase_if(m_devices, [&](const vk::PhysicalDevice &device) {
    return !device.supportsExtensions(extensions);
  });
  return *this;
}

auto PhysicalDeviceSelector::canPresentTo(const vk::khr::Surface &surface)
    -> PhysicalDeviceSelector & {
  std::erase_if(m_devices, [&](const vk::PhysicalDevice &device) {
    auto surfaceAttrs = vk::khr::SurfaceAttributes(device, surface);
    return surfaceAttrs.formats.empty() || surfaceAttrs.presentModes.empty();
  });
  return *this;
}

auto PhysicalDeviceSelector::rateDevices(
    const std::function<uint32_t(vk::PhysicalDevice &device)> &predicate)
    -> std::vector<DeviceRating> {
  std::vector<DeviceRating> ratings;
  ratings.reserve(m_devices.size());

  for (auto &device : m_devices) {
    uint32_t rating = predicate(device);
    if (rating > 0) {
      ratings.push_back({.device = device, .rating = rating});
    }
  }

  std::ranges::sort(ratings, [](const DeviceRating &a, const DeviceRating &b) {
    return a.rating > b.rating;
  });

  return ratings;
}
} // namespace engine
