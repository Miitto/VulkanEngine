#pragma once

#include <vulkan/vulkan_core.h>

namespace vk {
class Size;

class Offset {
  VkDeviceSize m_offset;

public:
  explicit Offset(VkDeviceSize offset = 0) noexcept : m_offset(offset) {}
  explicit Offset(const Size &size) noexcept;
  explicit Offset(uint32_t offset) noexcept
      : m_offset(static_cast<VkDeviceSize>(offset)) {}
  explicit Offset(int offset) noexcept
      : m_offset(static_cast<VkDeviceSize>(offset)) {}
  operator VkDeviceSize() const noexcept { return m_offset; }

  auto operator+(const Offset &other) const noexcept -> Offset {
    return Offset(m_offset + other.m_offset);
  }

  auto operator-(const Offset &other) const noexcept -> Offset {
    return Offset(m_offset - other.m_offset);
  }

  auto operator+(const Size &size) const noexcept -> Offset;

  auto operator-(const Size &size) const noexcept -> Offset;
};

class Offset2D : public VkOffset2D {
public:
  Offset2D(Offset x = Offset(0), Offset y = Offset(0))
      : VkOffset2D{.x = static_cast<int32_t>(x), .y = static_cast<int32_t>(y)} {
  }

  Offset2D(int32_t x = 0, int32_t y = 0) : VkOffset2D{.x = x, .y = y} {}
};

} // namespace vk
