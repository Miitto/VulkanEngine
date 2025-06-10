#include "offset.hpp"

#include "size.hpp"

namespace vk {
Offset::Offset(VkDeviceSize offset, VkDeviceSize alignment) noexcept
    // Round the offset up to the nearest alignment
    : m_offset(offset + (alignment - (offset % alignment))) {}

Offset::Offset(const Size &size) noexcept
    : m_offset(static_cast<VkDeviceSize>(size)) {}

auto Offset::operator+(const Size &size) const noexcept -> Offset {
  return Offset(m_offset + static_cast<VkDeviceSize>(size));
}

auto Offset::operator-(const Size &size) const noexcept -> Offset {
  return Offset(m_offset - static_cast<VkDeviceSize>(size));
}
} // namespace vk
