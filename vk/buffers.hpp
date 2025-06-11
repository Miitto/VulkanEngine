#pragma once

#include "handle.hpp"
#include "offset.hpp"
#include "ref.hpp"
#include "size.hpp"

#include "enums/buffer-usage.hpp"
#include "enums/index-type.hpp"
#include "enums/sharing-mode.hpp"

#include <optional>
#include <utility>
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
      Size size, enums::BufferUsage additionalUsage = enums::BufferUsage::None,
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
      Size size, enums::BufferUsage additionalUsage = enums::BufferUsage::None,
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
      Size size, enums::BufferUsage additionalUsage = enums::BufferUsage::None,
      enums::SharingMode sharingMode = enums::SharingMode::Exclusive)
      : BufferCreate(size, enums::BufferUsage::Uniform | additionalUsage,
                     sharingMode) {}
};

} // namespace info

class Buffer : public Handle<VkBuffer> {
protected:
  RawRef<Device, VkDevice> m_device;
  Size m_size;
  VkBufferUsageFlags m_usage;

  std::optional<MemoryRequirements> m_memoryRequirements;

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

  [[nodiscard]] constexpr virtual auto bufferTypeName() const -> const char * {
    return "Buffer";
  }

  auto getMemoryRequirements() -> MemoryRequirements;

  enum class BindError {
    OutOfHostMemory = VK_ERROR_OUT_OF_HOST_MEMORY,
    OutOfDeviceMemory = VK_ERROR_OUT_OF_DEVICE_MEMORY,
    InvalidOpaqueCaptureAddress = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR,
    MemoryTooSmall,
    AlignmentMismatch,
  };
  auto bind(DeviceMemory &memory, vk::Offset offset = Offset(0),
            bool align = false) -> std::optional<BindError>;

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
  enums::IndexType m_indexType;

  IndexBuffer(VkBuffer buffer, Device &device, Size size,
              VkBufferUsageFlags usage, enums::IndexType indexType) noexcept
      : Buffer(buffer, device, size, usage), m_indexType(indexType) {}

public:
  IndexBuffer(IndexBuffer &&o) noexcept
      : Buffer(std::move(o)), m_indexType(o.m_indexType) {}

  static auto create(Device &device, info::IndexBufferCreate &createInfo,
                     enums::IndexType indexType) -> std::optional<IndexBuffer>;
  [[nodiscard]] constexpr auto bufferTypeName() const -> const char * override {
    return "IndexBuffer";
  }

  [[nodiscard]] auto indexType() const -> enums::IndexType {
    return m_indexType;
  };
};

class UniformBuffer : public Buffer {
  UniformBuffer(VkBuffer buffer, Device &device, Size size,
                VkBufferUsageFlags usage) noexcept
      : Buffer(buffer, device, size, usage) {}

public:
  UniformBuffer(UniformBuffer &&o) noexcept : Buffer(std::move(o)) {}

  static auto create(Device &device, vk::info::UniformBufferCreate &createInfo)
      -> std::optional<UniformBuffer>;

  [[nodiscard]] constexpr auto bufferTypeName() const -> const char * override {
    return "UniformBuffer";
  }
};

class VertexBuffer : public Buffer {
  VertexBuffer(VkBuffer buffer, Device &device, Size size,
               VkBufferUsageFlags usage) noexcept
      : Buffer(buffer, device, size, usage) {}

public:
  VertexBuffer(VertexBuffer &&o) noexcept : Buffer(std::move(o)) {}

  static auto create(Device &device, vk::info::VertexBufferCreate &createInfo)
      -> std::optional<VertexBuffer>;

  [[nodiscard]] constexpr auto bufferTypeName() const -> const char * override {
    return "VertexBuffer";
  }
};

} // namespace vk

template <>
struct fmt::formatter<vk::Buffer::BindError> : fmt::formatter<const char *> {
  auto format(vk::Buffer::BindError my, format_context &ctx) const
      -> decltype(ctx.out()) {
    switch (my) {
    case vk::Buffer::BindError::OutOfHostMemory:
      return fmt::format_to(ctx.out(),
                            "vk::Buffer::BindError::OutOfHostMemory");
    case vk::Buffer::BindError::OutOfDeviceMemory:
      return fmt::format_to(ctx.out(),
                            "vk::Buffer::BindError::OutOfDeviceMemory");
    case vk::Buffer::BindError::InvalidOpaqueCaptureAddress:
      return fmt::format_to(
          ctx.out(), "vk::Buffer::BindError::InvalidOpaqueCaptureAddress");
    case vk::Buffer::BindError::MemoryTooSmall:
      return fmt::format_to(ctx.out(), "vk::Buffer::BindError::MemoryTooSmall");
    case vk::Buffer::BindError::AlignmentMismatch:
      return fmt::format_to(ctx.out(),
                            "vk::Buffer::BindError::AlignmentMismatch");
    }
    std::unreachable();
  }
};
