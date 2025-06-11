#include "device/device.hpp"

#include "commands/pool.hpp"
#include "descriptors.hpp"
#include "device/memory.hpp"
#include "device/physical.hpp"
#include "image-view.hpp"
#include "image.hpp"
#include "khr/swapchain.hpp"
#include "queue.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
auto Device::create(PhysicalDevice &physicalDevice,
                    vk::info::DeviceCreate &createInfo) noexcept
    -> std::optional<Device> {

  VkDevice device;
  VkResult result =
      vkCreateDevice(*physicalDevice, &createInfo, nullptr, &device);
  if (result != VK_SUCCESS) {
    return std::nullopt;
  }
  return Device(device, physicalDevice);
}

auto Device::getQueue(QueueFamily &family, uint32_t queueIndex)
    -> std::optional<Queue> {
  return getQueue(family.getIndex(), queueIndex);
}

auto Device::getQueue(int32_t queueFamilyIndex, uint32_t queueIndex)
    -> std::optional<Queue> {
  VkQueue queue;
  vkGetDeviceQueue(m_handle, queueFamilyIndex, queueIndex, &queue);
  if (queue == VK_NULL_HANDLE) {
    return std::nullopt;
  }
  return Queue(queue, queueFamilyIndex);
}

auto Device::createSwapchain(vk::info::SwapchainCreate &info)
    -> std::optional<khr::Swapchain> {
  return khr::Swapchain::create(*this, info);
}

auto Device::createCommandPool(vk::info::CommandPoolCreate &info)
    -> std::optional<CommandPool> {
  return CommandPool::create(*this, info);
}

auto Device::createSemaphore() -> std::optional<Semaphore> {
  return Semaphore::create(*this);
}

auto Device::createFence(bool signaled) -> std::optional<Fence> {
  return Fence::create(*this, signaled);
}

auto Device::createBuffer(vk::info::BufferCreate &info)
    -> std::optional<Buffer> {
  return Buffer::create(*this, info);
}

auto Device::createVertexBuffer(vk::info::VertexBufferCreate &info)
    -> std::optional<VertexBuffer> {
  return VertexBuffer::create(*this, info);
}

auto Device::createIndexBuffer(vk::info::IndexBufferCreate &info,
                               enums::IndexType indexType)
    -> std::optional<IndexBuffer> {
  return IndexBuffer::create(*this, info, indexType);
}

auto Device::createUniformBuffer(vk::info::UniformBufferCreate &info)
    -> std::optional<UniformBuffer> {
  return UniformBuffer::create(*this, info);
}

auto Device::allocateMemory(Buffer &buffer, enums::MemoryProperties properties)
    -> std::optional<DeviceMemory> {
  auto memoryReqs = buffer.getMemoryRequirements();

  return allocateMemory(memoryReqs, properties);
}

auto Device::allocateMemory(std::span<Buffer *> buffers,
                            enums::MemoryProperties properties)
    -> std::optional<DeviceMemory> {
  if (buffers.empty()) {
    return std::nullopt;
  }

  if (buffers.size() == 1) {
    return allocateMemory(*buffers[0], properties);
  }

  auto memoryReqs = buffers[0]->getMemoryRequirements();
  for (size_t i = 1; i < buffers.size(); i++) {
    MemoryRequirements reqs = buffers[i]->getMemoryRequirements();
    memoryReqs.size += reqs.size;
    memoryReqs.memoryTypeBits &= reqs.memoryTypeBits;
  }

  return allocateMemory(memoryReqs, properties);
}

auto Device::allocateMemory(MemoryRequirements reqs,
                            enums::MemoryProperties properties)
    -> std::optional<DeviceMemory> {
  auto memoryType =
      m_physicalDevice.findMemoryType(reqs.memoryTypeBits, properties);

  if (!memoryType) {
    return std::nullopt;
  }

  info::MemoryAllocate info(reqs.size, memoryType->index);

  return DeviceMemory::create(*this, info, memoryType.value());
}

void Device::bindBufferMemory(Buffer &buffer, DeviceMemory &memory,
                              uint32_t offset) {
  vkBindBufferMemory(m_handle, *buffer, *memory, offset);
}

auto Device::createDescriptorSetLayout(info::DescriptorSetLayoutCreate &info)
    -> std::optional<vk::DescriptorSetLayout> {
  return vk::DescriptorSetLayout::create(*this, info);
}

auto Device::createDescriptorPool(info::DescriptorPoolCreate &info)
    -> std::optional<vk::DescriptorPool> {
  return vk::DescriptorPool::create(*this, info);
}
} // namespace vk
