#pragma once

#include <spdlog/fmt/fmt.h>

#include <vulkan/vulkan_core.h>

namespace vk {
using DeviceSize = VkDeviceSize;
using Result = VkResult;

class Size {
  DeviceSize m_size;

public:
  explicit Size(DeviceSize size = 0) noexcept : m_size(size) {}
  explicit Size(uint32_t size) noexcept
      : m_size(static_cast<DeviceSize>(size)) {}
  explicit Size(long long unsigned int size) noexcept
      : m_size(static_cast<DeviceSize>(size)) {}
  explicit Size(int size) noexcept : m_size(static_cast<DeviceSize>(size)) {}
  operator DeviceSize() const noexcept { return m_size; }
};

} // namespace vk

template <> struct fmt::formatter<vk::Size> : fmt::formatter<int> {
  auto format(vk::Size my, format_context &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", static_cast<vk::DeviceSize>(my));
  }
};
