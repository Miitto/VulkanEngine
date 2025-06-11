#pragma once

#include "handle.hpp"
#include "ref.hpp"

#include "offset.hpp"
#include "size.hpp"

#include "device/physical.hpp"
#include "enums/memory-map.hpp"

#include "vulkan/vulkan_core.h"
#include <optional>
#include <span>
#include <vector>

namespace vk {
class Device;
class DeviceMemory;

namespace info {
class MemoryAllocate : public VkMemoryAllocateInfo {

public:
  MemoryAllocate(uint32_t allocSize, uint32_t memoryTypeIndex)
      : VkMemoryAllocateInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                             .pNext = nullptr,
                             .allocationSize = allocSize,
                             .memoryTypeIndex = memoryTypeIndex} {}
};
} // namespace info

class MappedMemoryRange : public VkMappedMemoryRange {
public:
  MappedMemoryRange(DeviceMemory &memory, Size size, Offset offset = Offset(0));
  MappedMemoryRange(const VkMappedMemoryRange &other)
      : VkMappedMemoryRange(other) {}
};

class Mapping : public Refable<Mapping> {
  RawRef<Device, VkDevice> m_device;
  RawRef<DeviceMemory, VkDeviceMemory> m_memory;
  Offset m_offset;
  Size m_size;
  void *m_ptr;

  bool m_isCoherent;

  struct Write {
    Offset start;
    Size size;
  };

  std::vector<Write> m_writes = {};

  Mapping(Device &device, DeviceMemory &memory, void *ptr, Offset offset,
          Size size);

public:
  Mapping() = delete;
  Mapping(const Mapping &) = delete;
  auto operator=(const Mapping &) -> Mapping & = delete;
  auto operator=(Mapping &&o) -> Mapping = delete;

  static auto map(Device &device, DeviceMemory &memory, Size size,
                  Offset offset = Offset(0),
                  enums::MemoryMap flags = enums::MemoryMap::None)
      -> std::optional<Mapping>;

  operator bool() const;
  Mapping(Mapping &&o) noexcept
      : Refable(std::move(o)), m_device(std::move(o.m_device)),
        m_memory(std::move(o.m_memory)), m_offset(o.m_offset), m_size(o.m_size),
        m_ptr(o.m_ptr), m_isCoherent(o.m_isCoherent) {
    o.m_ptr = nullptr;
    o.m_size = Size(0);
  }

  [[nodiscard]] auto get() const -> const void * { return m_ptr; }
  [[nodiscard]] auto getSize() const -> Size { return m_size; }

  template <typename T>
  auto write(std::span<T> data, Offset offset = Offset(0)) -> bool {
    Size size = Size(data.size() * sizeof(T));
    if (offset + size > m_size) {
      return false;
    }

    writeUnchecked(data.data(), size, offset);
    return true;
  }

  auto write(void *data, Size size, Offset offset = Offset(0)) -> bool {
    if (offset + size > m_size) {
      return false;
    }
    writeUnchecked(data, size, offset);
    return true;
  }

  void writeUnchecked(void *data, Size size, Offset offset) {
    memcpy(static_cast<char *>(m_ptr) + offset, data, size);

    registerWrite({.start = offset, .size = size});
  }

  [[nodiscard]] auto needsFlush() const -> bool { return !m_writes.empty(); }
  void registerWrite(Write write) {
    if (!m_isCoherent)
      m_writes.push_back(write);
  }

  void flush();

  ~Mapping();
};

class MappingSegment {
  Reference<Mapping> m_mapping;

  Offset m_offset;
  Size m_size;

public:
  MappingSegment() = delete;
  MappingSegment(Mapping &mapping, Offset offset, Size size)
      : m_mapping(mapping.ref()), m_offset(offset), m_size(size) {}

  MappingSegment(const MappingSegment &o) = default;
  auto operator=(const MappingSegment &o) -> MappingSegment & = default;
  MappingSegment(MappingSegment &&o) noexcept = default;
  auto operator=(MappingSegment &&o) noexcept -> MappingSegment & = default;

  [[nodiscard]] auto write(void *data, Size size, Offset offset = Offset(0))
      -> bool;
};

class DeviceMemory : public RawRefable<DeviceMemory, VkDeviceMemory>,
                     public Handle<VkDeviceMemory> {
  RawRef<Device, VkDevice> m_device;

  Size m_size;

  DeviceMemoryProperties m_memoryType;

  std::optional<Reference<Mapping>> m_mapping;

protected:
  friend class Mapping;

  auto activeMapping() -> std::optional<Reference<Mapping>> {
    if (m_mapping.has_value() && m_mapping.value().has_value()) {
      return m_mapping;
    } else {
      return std::nullopt;
    }
  }

public:
  DeviceMemory() = delete;
  DeviceMemory(const DeviceMemory &) = delete;
  auto operator=(const DeviceMemory &) -> DeviceMemory & = delete;

  DeviceMemory(Device &device, VkDeviceMemory memory, Size size,
               DeviceMemoryProperties &memoryType);
  DeviceMemory(DeviceMemory &&o) noexcept = default;

  auto destroy() -> void override;

  static auto create(Device &device, info::MemoryAllocate &info,
                     DeviceMemoryProperties &memoryType)
      -> std::optional<DeviceMemory>;

  [[nodiscard]] auto isCoherent() const -> bool {
    return (m_memoryType.memType.propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
  }

  [[nodiscard]] auto mappable() const -> bool {
    return (m_memoryType.memType.propertyFlags &
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
  }
  auto map(Size size = Size(VK_WHOLE_SIZE), Offset offset = Offset(0),
           enums::MemoryMap flags = enums::MemoryMap::None)
      -> std::optional<Mapping>;

  [[nodiscard]] auto getSize() const -> Size { return m_size; }
};

} // namespace vk
