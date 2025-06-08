#pragma once

#include "vk/device/physical.hpp"

namespace engine {
class PhysicalDeviceSelector;

class App {
protected:
  virtual auto selectPhysicalDevice(PhysicalDeviceSelector &physicalDevices)
      -> vk::PhysicalDevice;

public:
};
} // namespace engine
