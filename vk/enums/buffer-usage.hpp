#pragma once

#include <vulkan/vulkan_core.h>

namespace vk::enums {
class BufferUsage {
private:
  VkBufferUsageFlags flags;

public:
  explicit constexpr BufferUsage(VkBufferUsageFlags flags) : flags(flags) {}

  operator VkBufferUsageFlags() const { return flags; }

  auto operator|(BufferUsage other) const -> BufferUsage {
    return BufferUsage{flags | other.flags};
  }

  static const BufferUsage None, Vertex, Index, Uniform, Storage, TransferSrc,
      TransferDst;
};
} // namespace vk::enums
