#pragma once

#include <bit>
#include <vulkan/vulkan_core.h>

namespace vk::enums {
class BufferUsage {
public:
  enum Bits {
    Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
  };

private:
  VkBufferUsageFlags flags;

public:
  BufferUsage(VkBufferUsageFlags flags) : flags(flags) {}
  BufferUsage(Bits bits) : flags(std::bit_cast<VkBufferUsageFlags>(bits)) {}

  operator VkBufferUsageFlags() const { return flags; }

  auto operator|(BufferUsage other) const -> BufferUsage {
    return {flags | other.flags};
  }
};
} // namespace vk::enums
