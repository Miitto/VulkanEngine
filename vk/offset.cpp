#include "offset.hpp"

#include "size.hpp"

namespace vk {
Offset::Offset(const Size &size) noexcept
    : m_offset(static_cast<VkDeviceSize>(size)) {}

auto Offset::operator+(const Size &size) const noexcept -> Offset {
  return Offset(m_offset + static_cast<VkDeviceSize>(size));
}

auto Offset::operator-(const Size &size) const noexcept -> Offset {
  return Offset(m_offset - static_cast<VkDeviceSize>(size));
}
} // namespace vk
