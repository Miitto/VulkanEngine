#pragma once

#include "handle.hpp"
#include "ref.hpp"
#include "size.hpp"

#include "enums/buffer-usage-flags.hpp"
#include "enums/sharing-mode.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;
class DeviceMemory;
} // namespace vk

namespace vk {

class BufferCopy : public VkBufferCopy {
public:
  BufferCopy(uint32_t srcOffset = 0, uint32_t dstOffset = 0, uint32_t size = 0)
      : VkBufferCopy{
            .srcOffset = srcOffset, .dstOffset = dstOffset, .size = size} {}
  BufferCopy(const VkBufferCopy &o) : VkBufferCopy(o) {}

  auto setSrcOffset(uint32_t offset) -> BufferCopy & {
    srcOffset = offset;
    return *this;
  }

  auto setDstOffset(uint32_t offset) -> BufferCopy & {
    dstOffset = offset;
    return *this;
  }

  auto setSize(uint32_t sz) -> BufferCopy & {
    size = sz;
    return *this;
  }
};

class MemoryRequirements : public VkMemoryRequirements {
public:
  MemoryRequirements(const VkMemoryRequirements &other)
      : VkMemoryRequirements(other) {}
};

namespace info {
class BufferCreate : public VkBufferCreateInfo {

public:
  BufferCreate()
      : VkBufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                           .pNext = nullptr,
                           .flags = 0,
                           .size = 0,
                           .usage = 0,
                           .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                           .queueFamilyIndexCount = 0,
                           .pQueueFamilyIndices = nullptr} {}
  BufferCreate(Size size, enums::BufferUsage usage,
               enums::SharingMode sharingMode = enums::SharingMode::Exclusive)
      : BufferCreate() {
    this->usage = usage;
    this->size = size;
    this->sharingMode = sharingMode;
  }
};

class VertexBufferCreate : public BufferCreate {
public:
  VertexBufferCreate() : BufferCreate() {
    usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  }

  VertexBufferCreate(
      Size size, enums::BufferUsage additionalUsage = 0,
      enums::SharingMode sharingMode = enums::SharingMode::Exclusive)
      : BufferCreate(size, enums::BufferUsage::Vertex | additionalUsage,
                     sharingMode) {}
};
class IndexBufferCreate : public BufferCreate {
public:
  IndexBufferCreate() : BufferCreate() {
    usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }

  IndexBufferCreate(
      Size size, enums::BufferUsage additionalUsage = 0,
      enums::SharingMode sharingMode = enums::SharingMode::Exclusive)
      : BufferCreate(size, enums::BufferUsage::Index | additionalUsage,
                     sharingMode) {}
};

class UniformBufferCreate : public BufferCreate {
public:
  UniformBufferCreate() : BufferCreate() {
    usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  }

  UniformBufferCreate(
      Size size, enums::BufferUsage additionalUsage = 0,
      enums::SharingMode sharingMode = enums::SharingMode::Exclusive)
      : BufferCreate(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | additionalUsage,
                     sharingMode) {}
};

} // namespace info

class Buffer : public Handle<VkBuffer> {
protected:
  RawRef<Device, VkDevice> m_device;
  Size m_size;
  VkBufferUsageFlags m_usage;

  struct MemoryLocation {
    RawRef<DeviceMemory, VkDeviceMemory> memory;
    VkDeviceSize offset;
  };

  std::optional<MemoryLocation> m_memory;

  Buffer(VkBuffer buffer, Device &device, Size size,
         VkBufferUsageFlags usage) noexcept;

public:
  Buffer(Buffer &&o) noexcept = default;

  static auto create(Device &device, info::BufferCreate &createInfo)
      -> std::optional<Buffer>;
  auto destroy() -> void override;

  auto size() -> VkDeviceSize { return m_size; }

  auto getDevice() -> RawRef<Device, VkDevice> & { return m_device; }

  auto getMemoryRequirements() -> MemoryRequirements;

  auto bind(DeviceMemory &memory, VkDeviceSize offset = 0) -> VkResult;

  [[nodiscard]] auto isBound() const -> bool;

  [[nodiscard]] auto canMap() const -> bool;

  [[nodiscard]] auto canCopyFrom() const -> bool {
    return isBound() && m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  }

  [[nodiscard]] auto canCopyTo() const -> bool {
    return isBound() && (m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) != 0;
  }
};

class IndexBuffer : public Buffer {
  IndexBuffer(VkBuffer buffer, Device &device, Size size,
              VkBufferUsageFlags usage) noexcept
      : Buffer(buffer, device, size, usage) {}

public:
  IndexBuffer(IndexBuffer &&o) noexcept : Buffer(std::move(o)) {}

  static auto create(Device &device, vk::info::IndexBufferCreate &createInfo)
      -> std::optional<IndexBuffer>;
};

class UniformBuffer : public Buffer {
  UniformBuffer(VkBuffer buffer, Device &device, Size size,
                VkBufferUsageFlags usage) noexcept
      : Buffer(buffer, device, size, usage) {}

public:
  UniformBuffer(UniformBuffer &&o) noexcept : Buffer(std::move(o)) {}

  static auto create(Device &device, vk::info::UniformBufferCreate &createInfo)
      -> std::optional<UniformBuffer>;
};

class VertexBuffer : public Buffer {
  VertexBuffer(VkBuffer buffer, Device &device, Size size,
               VkBufferUsageFlags usage) noexcept
      : Buffer(buffer, device, size, usage) {}

public:
  VertexBuffer(VertexBuffer &&o) noexcept : Buffer(std::move(o)) {}

  static auto create(Device &device, vk::info::VertexBufferCreate &createInfo)
      -> std::optional<VertexBuffer>;
};

} // namespace vk
