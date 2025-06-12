#include "buffers.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"
#include "device/memory.hpp"
#include "enums/index-type.hpp"

namespace vk {

Buffer::Buffer(VkBuffer buffer, Device &device, Size size,
               VkBufferUsageFlags usage) noexcept
    : Handle<VkBuffer>(buffer), m_device(device.ref()), m_size(size),
      m_usage(usage) {}

auto Buffer::create(Device &device, vk::info::BufferCreate &createInfo)
    -> std::optional<Buffer> {
  VkBuffer buffer;
  if (vkCreateBuffer(device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
    return std::nullopt;
  }

  return Buffer(buffer, device, Size(createInfo.size), createInfo.usage);
}

auto Buffer::destroy() -> void { vkDestroyBuffer(m_device, m_handle, nullptr); }

auto Buffer::bind(DeviceMemory &memory, vk::Offset offset, bool align)
    -> std::optional<BindError> {
  if (align) {
    offset.alignTo(getMemoryRequirements().alignment);
  }
#ifndef NDEBUG
  auto memoryRequirements = getMemoryRequirements();
  if (memoryRequirements.size < m_size) {
    Logger::error("Buffer size {} is larger than memory size {}", m_size,
                  memoryRequirements.size);
    return BindError::MemoryTooSmall;
  }

  if (offset % memoryRequirements.alignment != 0) {
    Logger::error("Can't bind buffer at offset {} as is not a multiple of the "
                  "buffer alignment ({})",
                  offset, memoryRequirements.alignment);
    return BindError::AlignmentMismatch;
  }
#endif
  Logger::debug("Binding {} of size {} with offset {}. Alignment: {}",
                bufferTypeName(), m_size, offset, memoryRequirements.alignment);

  auto res = vkBindBufferMemory(m_device, m_handle, memory, offset);

  if (res != VK_SUCCESS) {
    return std::bit_cast<BindError>(res);
  }

  m_memory = {.memory = memory.ref(), .offset = offset};

  return std::nullopt;
}

auto Buffer::getMemoryRequirements() -> MemoryRequirements {
  if (!m_memoryRequirements.has_value()) {
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(**m_device, m_handle, &memoryRequirements);
    m_memoryRequirements = memoryRequirements;
  }
  return m_memoryRequirements.value();
}

auto Buffer::isBound() const -> bool {
  return m_memory.has_value() && m_memory.value().memory.has_value();
}

auto Buffer::canMap() const -> bool {
  return isBound() && m_memory.value().memory.value().mappable();
}

auto IndexBuffer::create(Device &device, info::IndexBufferCreate &createInfo,
                         IndexType indexType) -> std::optional<IndexBuffer> {
  VkBuffer buffer;
  if (vkCreateBuffer(*device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
    return std::nullopt;
  }

  return IndexBuffer(buffer, device, Size(createInfo.size), createInfo.usage,
                     indexType);
}

auto UniformBuffer::create(Device &device,
                           vk::info::UniformBufferCreate &createInfo)
    -> std::optional<UniformBuffer> {
  VkBuffer buffer;
  if (vkCreateBuffer(*device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
    return std::nullopt;
  }

  return UniformBuffer(buffer, device, Size(createInfo.size), createInfo.usage);
}

auto VertexBuffer::create(Device &device,
                          vk::info::VertexBufferCreate &createInfo)
    -> std::optional<VertexBuffer> {
  VkBuffer buffer;
  if (vkCreateBuffer(*device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
    return std::nullopt;
  }

  return VertexBuffer(buffer, device, Size(createInfo.size), createInfo.usage);
}
} // namespace vk
