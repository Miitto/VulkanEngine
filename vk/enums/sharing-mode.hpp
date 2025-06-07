#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class SharingMode {
public:
  enum Values {
    Exclusive = VK_SHARING_MODE_EXCLUSIVE,
    Concurrent = VK_SHARING_MODE_CONCURRENT,
  };

private:
  Values value;

public:
  SharingMode(Values value) : value(value) {}

  operator VkSharingMode() const { return std::bit_cast<VkSharingMode>(value); }
};
} // namespace vk::enums
