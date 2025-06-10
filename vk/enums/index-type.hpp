#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class IndexType {
public:
  enum Values {
    U16 = VK_INDEX_TYPE_UINT16,
    U32 = VK_INDEX_TYPE_UINT32,
  };

private:
  Values value;

public:
  IndexType(Values v) : value(v) {}
  operator VkIndexType() const { return std::bit_cast<VkIndexType>(value); }
};
} // namespace vk::enums
