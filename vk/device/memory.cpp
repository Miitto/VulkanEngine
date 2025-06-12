#include "device/memory.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
MappedMemoryRange::MappedMemoryRange(DeviceMemory &memory, Size size,
                                     Offset offset)
    : VkMappedMemoryRange{.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                          .pNext = nullptr,
                          .memory = memory,
                          .offset = offset,
                          .size = size} {}

Mapping::Mapping(Device &device, DeviceMemory &memory, void *ptr, Offset offset,
                 Size size)
    : Refable(), m_device(device.ref()), m_memory(memory.ref()),
      m_offset(offset), m_size(size), m_ptr(ptr),
      m_isCoherent(memory.isCoherent()) {}

void Mapping::flush() {
  if (needsFlush()) {
    std::vector<MappedMemoryRange> ranges;
    ranges.reserve(m_writes.size());
    for (auto &write : m_writes) {
      ranges.emplace_back(m_memory, write.size, write.start);
    }
    vkFlushMappedMemoryRanges(m_device, static_cast<uint32_t>(ranges.size()),
                              ranges.data());

    m_writes.clear();
  }
}

Mapping::operator bool() const {
  return m_ptr != nullptr && m_size > 0 && m_memory.has_value() &&
         m_device.has_value();
}

auto MappingSegment::write(void *data, Size size, Offset offset) -> bool {
  if (!m_mapping.has_value()) {
    Logger::error("Mapping is not valid");
    return false;
  }
  if (offset + size > m_size) {
    Logger::error("Write size {} exceeds segment size {}", size, m_size);
    return false;
  }
  auto &mapping = *m_mapping;

  mapping.writeUnchecked(data, size, m_offset + offset);
  return true;
}

DeviceMemory::DeviceMemory(Device &device, VkDeviceMemory memory, Size size,
                           DeviceMemoryProperties &memoryType)
    : RawRefable(), Handle(memory), m_device(device.ref()), m_size(size),
      m_memoryType(memoryType) {}

auto DeviceMemory::create(Device &device, info::MemoryAllocate &info,
                          DeviceMemoryProperties &memoryType)
    -> std::optional<DeviceMemory> {
  VkDeviceMemory memory;
  if (vkAllocateMemory(*device, &info, nullptr, &memory) != VK_SUCCESS) {
    return std::nullopt;
  }

  DeviceMemory mem(device, memory, Size(info.allocationSize), memoryType);

  return mem;
}

auto DeviceMemory::destroy() -> void {
  vkFreeMemory(m_device, m_handle, nullptr);
}

auto Mapping::map(Device &device, DeviceMemory &memory, Size size,
                  Offset offset, MemoryMapFlags flags)
    -> std::optional<Mapping> {
  auto activeMapping = memory.activeMapping();
  if (activeMapping.has_value()) {
    Logger::error("Memory is already mapped");
    return std::nullopt;
  }

  void *ptr;
  if (vkMapMemory(*device, *memory, offset, size, flags, &ptr) != VK_SUCCESS) {
    return std::nullopt;
  }

  return Mapping(device, memory, ptr, offset,
                 size == VK_WHOLE_SIZE ? memory.getSize() : size);
}

Mapping::~Mapping() {
  if (m_ptr != nullptr) {
    flush();
    vkUnmapMemory(m_device, m_memory);
  }
}

auto DeviceMemory::map(Size size, Offset offset, MemoryMapFlags flags)
    -> std::optional<Mapping> {
  if (!m_handle) {
    Logger::error("Memory is not valid.");
    return std::nullopt;
  }

  if (!mappable()) {
    Logger::error("Memory is not mappable.");
    return std::nullopt;
  }

  // Ensure that the requested size and offset are within the bounds of the
  // buffer and the device memory
  if (m_size < (size == VK_WHOLE_SIZE ? m_size : size) + offset) {
    Logger::error("Memory is smaller than requested size.");
    return std::nullopt;
  }

  return Mapping::map(m_device, *this, size, offset, flags);
}

} // namespace vk
