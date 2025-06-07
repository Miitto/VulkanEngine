#include "buffers.hpp"

#include "device/device.hpp"
#include "device/memory.hpp"

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

auto Buffer::bind(DeviceMemory &memory, VkDeviceSize offset) -> VkResult {
  auto res = vkBindBufferMemory(m_device, m_handle, memory, offset);

  if (res != VK_SUCCESS) {
    return res;
  }

  m_memory = {.memory = memory.ref(), .offset = offset};

  return res;
}

auto Buffer::getMemoryRequirements() -> MemoryRequirements {
  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(**m_device, m_handle, &memoryRequirements);
  return memoryRequirements;
}

auto Buffer::isBound() const -> bool {
  return m_memory.has_value() && m_memory.value().memory.has_value();
}

auto Buffer::canMap() const -> bool {
  return isBound() && m_memory.value().memory.value().mappable();
}

auto IndexBuffer::create(Device &device,
                         vk::info::IndexBufferCreate &createInfo)
    -> std::optional<IndexBuffer> {
  VkBuffer buffer;
  if (vkCreateBuffer(*device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
    return std::nullopt;
  }

  return IndexBuffer(buffer, device, Size(createInfo.size), createInfo.usage);
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
