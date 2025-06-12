#pragma once

#include <vulkan/vulkan_core.h>

namespace vk {
enum class BufferUsage : VkBufferUsageFlags {
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
} // namespace vk
