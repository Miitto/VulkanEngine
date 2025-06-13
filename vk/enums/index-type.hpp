#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk {
class IndexType {
public:
  enum Flags {
    None = VK_INDEX_TYPE_NONE_KHR,
    U8 = VK_INDEX_TYPE_UINT8_EXT,
    U16 = VK_INDEX_TYPE_UINT16,
    U32 = VK_INDEX_TYPE_UINT32,
  };

private:
  Flags m_flag;

public:
  IndexType() : m_flag(None) {}
  IndexType(Flags flag) : m_flag(flag) {}

  operator VkIndexType() const { return std::bit_cast<VkIndexType>(m_flag); }
};

} // namespace vk
