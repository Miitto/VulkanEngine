#pragma once

#include <spdlog/fmt/fmt.h>
#include <vulkan/vulkan_core.h>

namespace vk {
class Size;

class Offset {
  VkDeviceSize m_offset;

public:
  explicit Offset(VkDeviceSize offset = 0) noexcept : m_offset(offset) {}
  explicit Offset(VkDeviceSize offset, VkDeviceSize alignment) noexcept;
  explicit Offset(const Size &size) noexcept;
  explicit Offset(uint32_t offset) noexcept
      : m_offset(static_cast<VkDeviceSize>(offset)) {}
  explicit Offset(int offset) noexcept
      : m_offset(static_cast<VkDeviceSize>(offset)) {}
  operator VkDeviceSize() const noexcept { return m_offset; }

  constexpr inline auto alignTo(VkDeviceSize alignment) noexcept -> Offset & {
    if (alignment == 0) {
      return *this;
    }
    m_offset += (alignment - (m_offset % alignment));
    return *this;
  }

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

template <> struct fmt::formatter<vk::Offset> : fmt::formatter<int> {
  auto format(vk::Offset my, format_context &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "Offset({})",
                          static_cast<VkDeviceSize>(my));
  }
};
