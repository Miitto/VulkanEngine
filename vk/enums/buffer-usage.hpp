#pragma once

#include "util/vk-logger.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vk {
class BufferUsage {
public:
  enum Bits : VkBufferUsageFlags {
    None = 0,
    TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
    ShaderDeviceAddress = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
  };

private:
  VkBufferUsageFlags m_flags;

  static constexpr VkBufferUsageFlags allFlags =
      TransferSrc | TransferDst | UniformTexelBuffer | StorageTexelBuffer |
      UniformBuffer | StorageBuffer | IndexBuffer | VertexBuffer |
      IndirectBuffer | ShaderDeviceAddress;

public:
  constexpr BufferUsage() : m_flags(None) {}
  constexpr BufferUsage(const VkBufferUsageFlags flags) : m_flags(flags) {
    if (flags & ~allFlags) {
      if (std::is_constant_evaluated()) {
        throw std::invalid_argument("Invalid VkBufferUsageFlags provided");
      } else {
        Logger::error(
            "Invalid VkBufferUsageFlags provided: {:b}. Valid Mask: {:b}",
            flags, allFlags);
      }
    }
  }
  constexpr BufferUsage(Bits bits) : m_flags(bits) {}

  constexpr operator VkBufferUsageFlags() const { return m_flags; }
  constexpr auto operator|(Bits bits) const -> BufferUsage {
    return (m_flags | bits);
  }

  constexpr auto operator|(const BufferUsage &other) const -> BufferUsage {
    return (m_flags | other.m_flags);
  }
};
} // namespace vk

constexpr auto operator|(vk::BufferUsage::Bits bits, vk::BufferUsage usage) {
  return usage | bits;
}
