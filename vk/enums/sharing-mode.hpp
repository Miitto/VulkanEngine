#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
enum class SharingMode {
  Exclusive = VK_SHARING_MODE_EXCLUSIVE,
  Concurrent = VK_SHARING_MODE_CONCURRENT,
};
} // namespace vk
