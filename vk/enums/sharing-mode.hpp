#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
class SharingMode {
public:
  enum Flags {
    Exclusive = VK_SHARING_MODE_EXCLUSIVE,
    Concurrent = VK_SHARING_MODE_CONCURRENT,
  };

private:
  Flags m_flag;

public:
  SharingMode() : m_flag(Exclusive) {}
  SharingMode(Flags flag) : m_flag(flag) {}

  operator VkSharingMode() const {
    return std::bit_cast<VkSharingMode>(m_flag);
  }
};
} // namespace vk
