#include "descriptors.hpp"

#include "util/vk-logger.hpp"

#include "buffers.hpp"
#include "device/device.hpp"
#include "enums/shader-stage.hpp"

#include <optional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
// Region Layouts

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
    uint32_t binding, DescriptorType type, ShaderStageFlags stageFlags,
    uint32_t count)
    : VkDescriptorSetLayoutBinding{.binding = binding,
                                   .descriptorType = type,
                                   .descriptorCount = count,
                                   .stageFlags = stageFlags,
                                   .pImmutableSamplers = nullptr} {}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
    VkDescriptorSetLayoutBinding &&other)
    : VkDescriptorSetLayoutBinding(other) {}

namespace info {
DescriptorSetLayoutCreate::DescriptorSetLayoutCreate()
    : VkDescriptorSetLayoutCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .bindingCount = 0,
          .pBindings = nullptr} {}

DescriptorSetLayoutCreate::DescriptorSetLayoutCreate(
    VkDescriptorSetLayoutCreateInfo &&other)
    : VkDescriptorSetLayoutCreateInfo(other) {}

DescriptorSetLayoutCreate &DescriptorSetLayoutCreate::addBinding(
    const DescriptorSetLayoutBinding &binding) {
  bindings.push_back(binding);
  setupBindings();
  return *this;
}
} // namespace info

DescriptorSetLayout::DescriptorSetLayout(VkDescriptorSetLayout &layout,
                                         Device &device)
    : RawRefable(), m_layout(layout), m_device(device.ref()) {}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
    : RawRefable<DescriptorSetLayout, VkDescriptorSetLayout>(std::move(other)),
      m_layout(other.m_layout), m_device(other.m_device) {
  other.m_layout = VK_NULL_HANDLE;
}

DescriptorSetLayout::~DescriptorSetLayout() {
  if (m_layout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(*m_device, m_layout, nullptr);
    m_layout = VK_NULL_HANDLE;
  }
}

std::optional<DescriptorSetLayout>
DescriptorSetLayout::create(Device &device,
                            info::DescriptorSetLayoutCreate &createInfo) {
  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(*device, &createInfo, nullptr, &layout) !=
      VK_SUCCESS) {
    return std::nullopt;
  }

  return DescriptorSetLayout(layout, device);
}

// Region Pool

DescriptorPoolSize::DescriptorPoolSize(DescriptorType type, uint32_t count)
    : VkDescriptorPoolSize{.type = type, .descriptorCount = count} {}

namespace info {
DescriptorPoolCreate::DescriptorPoolCreate(uint32_t maxSets)
    : VkDescriptorPoolCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .maxSets = maxSets,
          .poolSizeCount = 0,
          .pPoolSizes = nullptr} {}

DescriptorPoolCreate::DescriptorPoolCreate(VkDescriptorPoolCreateInfo &&other)
    : VkDescriptorPoolCreateInfo(other) {}

} // namespace info

DescriptorPool::DescriptorPool(VkDescriptorPool handle, Device &device)
    : m_handle(handle), device(device.ref()) {}

DescriptorPool::DescriptorPool(DescriptorPool &&other) noexcept
    : m_handle(other.m_handle), device(other.device) {
  other.m_handle = VK_NULL_HANDLE;
}

DescriptorPool::~DescriptorPool() {
  if (m_handle != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(*device, m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
  }
}

std::optional<DescriptorPool>
DescriptorPool::create(Device &device, info::DescriptorPoolCreate &info) {
  VkDescriptorPool pool;
  if (vkCreateDescriptorPool(*device, &info, nullptr, &pool) != VK_SUCCESS) {
    return std::nullopt;
  }
  return DescriptorPool(pool, device);
}

std::optional<std::vector<DescriptorSet>>
DescriptorPool::allocateSets(DescriptorSetLayout &layout, uint32_t count) {
  auto ref = layout.ref();
  std::vector<DescriptorSetLayout::Ref> layoutsVec(count, ref);
  std::span<DescriptorSetLayout::Ref> layoutsSpan(layoutsVec);

  return allocateSets(layoutsSpan);
}

std::optional<std::vector<DescriptorSet>>
DescriptorPool::allocateSets(std::span<DescriptorSetLayout::Ref> &layouts) {
  if (layouts.empty()) {
    return std::nullopt;
  }

  info::DescriptorSetAllocate allocInfo(*this);
  allocInfo.addLayouts(layouts);

  std::vector<VkDescriptorSet> sets;
  sets.resize(layouts.size());

  if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) != VK_SUCCESS) {
    Logger::error("Failed to allocate descriptor sets.");
    return std::nullopt;
  }

  std::vector<DescriptorSet> descriptorSets;
  descriptorSets.reserve(sets.size());
  for (auto &set : sets) {
    descriptorSets.emplace_back(set, device);
  }

  return descriptorSets;
}

// Descriptor Sets

namespace info {
DescriptorSetAllocate::DescriptorSetAllocate()
    : VkDescriptorSetAllocateInfo{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .pNext = nullptr,
          .descriptorPool = VK_NULL_HANDLE,
          .descriptorSetCount = 0,
          .pSetLayouts = nullptr} {}

DescriptorSetAllocate::DescriptorSetAllocate(DescriptorPool &pool)
    : DescriptorSetAllocate() {
  setDescriptorPool(pool);
}

DescriptorSetAllocate::DescriptorSetAllocate(
    VkDescriptorSetAllocateInfo &&other)
    : VkDescriptorSetAllocateInfo(other) {}

DescriptorBuffer::DescriptorBuffer(Buffer buf, VkDescriptorType bufType,
                                   VkDeviceSize offset, VkDeviceSize range)
    : VkDescriptorBufferInfo{.buffer = buf, .offset = offset, .range = range},
      m_bufferType(bufType) {}

DescriptorBuffer::DescriptorBuffer(UniformBuffer &buf, VkDeviceSize offset,
                                   VkDeviceSize range)
    : VkDescriptorBufferInfo{.buffer = buf, .offset = offset, .range = range},
      m_bufferType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {}

} // namespace info

DescriptorSet::DescriptorSet(VkDescriptorSet set, Device &device)
    : m_handle(set), m_device(device.ref()) {}

void DescriptorSet::update(DescriptorSetWrite &write) {
  vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

DescriptorSetWrite::DescriptorSetWrite(DescriptorSet &set, uint32_t binding)
    : VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                           .pNext = nullptr,
                           .dstSet = set,
                           .dstBinding = binding,
                           .dstArrayElement = 0,
                           .descriptorCount = 0,
                           .descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM,
                           .pImageInfo = nullptr,
                           .pBufferInfo = nullptr,
                           .pTexelBufferView = nullptr} {}

DescriptorSetWriteBuffer::DescriptorSetWriteBuffer(DescriptorSet &set,
                                                   uint32_t binding)
    : DescriptorSetWrite(set, binding) {}

DescriptorSetWriteBuffer::DescriptorSetWriteBuffer(DescriptorSet &set,
                                                   uint32_t binding,
                                                   UniformBuffer &buffer,
                                                   uint32_t offset,
                                                   VkDeviceSize range)
    : DescriptorSetWrite(set, binding) {
  m_buffers.push_back(info::DescriptorBuffer(buffer, offset, range));
  setupBuffers();
}

auto DescriptorSetWriteBuffer::addBuffer(info::DescriptorBuffer &buffer)
    -> DescriptorSetWriteBuffer & {
  if (descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM) {
    descriptorType = buffer.bufferType();
  }
  m_buffers.push_back(buffer);
  setupBuffers();
  return *this;
}
} // namespace vk
