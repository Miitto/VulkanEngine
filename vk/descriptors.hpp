#pragma once

#include "ref.hpp"

#include <bit>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;
class Buffer;
class UniformBuffer;
} // namespace vk

namespace vk {
class DescriptorSetLayoutBinding : public VkDescriptorSetLayoutBinding {
public:
  DescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type,
                             VkShaderStageFlags stageFlags, uint32_t count = 1);
  DescriptorSetLayoutBinding(VkDescriptorSetLayoutBinding &&other);
};

namespace info {
class DescriptorSetLayoutCreate : public VkDescriptorSetLayoutCreateInfo {
  std::vector<DescriptorSetLayoutBinding> bindings;

  inline void setupBindings() {
    bindingCount = static_cast<uint32_t>(bindings.size());
    pBindings = bindings.data();
  }

public:
  DescriptorSetLayoutCreate();

  DescriptorSetLayoutCreate(VkDescriptorSetLayoutCreateInfo &&other);

  auto addBinding(const DescriptorSetLayoutBinding &binding)
      -> DescriptorSetLayoutCreate &;
};
} // namespace info

class DescriptorSetLayout
    : public RawRefable<DescriptorSetLayout, VkDescriptorSetLayout> {
  VkDescriptorSetLayout m_layout;
  RawRef<Device, VkDevice> m_device;

  DescriptorSetLayout(VkDescriptorSetLayout &layout, Device &device);

public:
  DescriptorSetLayout() = delete;
  DescriptorSetLayout(const DescriptorSetLayout &) = delete;
  auto operator=(const DescriptorSetLayout &) -> DescriptorSetLayout & = delete;

  using Ref = RawRef<DescriptorSetLayout, VkDescriptorSetLayout>;

  DescriptorSetLayout(DescriptorSetLayout &&other) noexcept;

  static auto create(Device &device,
                     info::DescriptorSetLayoutCreate &createInfo)
      -> std::optional<DescriptorSetLayout>;

  ~DescriptorSetLayout();

  auto operator*() -> VkDescriptorSetLayout { return m_layout; }
  operator VkDescriptorSetLayout() { return m_layout; }

  auto device() -> RawRef<Device, VkDevice> { return m_device; }
};

namespace enums {
class DescriptorType {
public:
  enum Values : uint32_t {
    Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
    CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    UniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    StorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
  };

private:
  Values value;

public:
  DescriptorType(Values v) : value(v) {}
  DescriptorType(uint32_t v) : value(static_cast<Values>(v)) {}
  DescriptorType(VkDescriptorType v) : value(std::bit_cast<Values>(v)) {}

  operator VkDescriptorType() const {
    return std::bit_cast<VkDescriptorType>(value);
  }

  operator Values() const { return value; }
};
} // namespace enums

class DescriptorPoolSize : public VkDescriptorPoolSize {
public:
  DescriptorPoolSize(enums::DescriptorType type, uint32_t count);

  inline DescriptorPoolSize &setCount(uint32_t count) {
    descriptorCount = count;
    return *this;
  }

  inline DescriptorPoolSize &setType(enums::DescriptorType t) {
    this->type = t;
    return *this;
  }
};

namespace info {
class DescriptorPoolCreate : public VkDescriptorPoolCreateInfo {
  std::vector<DescriptorPoolSize> poolSizes{};

  inline void setupPoolSizes() {
    poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    pPoolSizes = poolSizes.data();
  }

public:
  DescriptorPoolCreate(uint32_t maxSets = 0);
  DescriptorPoolCreate(VkDescriptorPoolCreateInfo &&other);

  DescriptorPoolCreate &addPoolSize(const DescriptorPoolSize &poolSize) {
    poolSizes.push_back(poolSize);
    setupPoolSizes();
    return *this;
  }

  DescriptorPoolCreate &setMaxSets(uint32_t maxSets) {
    this->maxSets = maxSets;
    return *this;
  }
};
} // namespace info

class DescriptorSet;

class DescriptorPool {
  VkDescriptorPool m_handle;
  RawRef<Device, VkDevice> device;

  DescriptorPool(VkDescriptorPool handle, Device &device);

public:
  DescriptorPool() = delete;
  DescriptorPool(const DescriptorPool &) = delete;
  DescriptorPool &operator=(const DescriptorPool &) = delete;

  DescriptorPool(DescriptorPool &&other) noexcept;

  static std::optional<DescriptorPool> create(Device &device,
                                              info::DescriptorPoolCreate &info);
  ~DescriptorPool();

  VkDescriptorPool operator*() { return m_handle; }
  operator VkDescriptorPool() { return m_handle; }

  std::optional<std::vector<DescriptorSet>>
  allocateSets(std::span<DescriptorSetLayout::Ref> &layouts);
  std::optional<std::vector<DescriptorSet>>
  allocateSets(DescriptorSetLayout &layout, uint32_t count = 1);
};

namespace info {
class DescriptorSetAllocate : public VkDescriptorSetAllocateInfo {
  std::vector<DescriptorSetLayout::Ref> layouts{};
  std::vector<VkDescriptorSetLayout> pLayouts{};

  void setupLayouts() {
    pLayouts.clear();
    pLayouts.reserve(layouts.size());

    for (const auto &layout : layouts) {
      pLayouts.push_back(*layout);
    }

    descriptorSetCount = static_cast<uint32_t>(pLayouts.size());
    pSetLayouts = pLayouts.data();
  }

public:
  DescriptorSetAllocate();
  DescriptorSetAllocate(DescriptorPool &pool);
  DescriptorSetAllocate(VkDescriptorSetAllocateInfo &&other);

  DescriptorSetAllocate &setDescriptorPool(DescriptorPool &pool) {
    descriptorPool = pool;
    return *this;
  }

  DescriptorSetAllocate &addLayout(DescriptorSetLayout &layout) {
    layouts.push_back(layout.ref());
    setupLayouts();
    return *this;
  }
  DescriptorSetAllocate &
  addLayouts(std::span<DescriptorSetLayout::Ref> &layoutSpan) {
    for (auto &layout : layoutSpan) {
      layouts.push_back(layout);
    }
    setupLayouts();
    return *this;
  }
};

class DescriptorBuffer : public VkDescriptorBufferInfo {
  VkDescriptorType m_bufferType;

public:
  DescriptorBuffer(Buffer buf, VkDescriptorType bufType,
                   VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

  DescriptorBuffer(UniformBuffer &buf, VkDeviceSize offset = 0,
                   VkDeviceSize range = VK_WHOLE_SIZE);

  VkDescriptorType bufferType() const { return m_bufferType; }
};

} // namespace info

class DescriptorSetWrite;

class DescriptorSet {
  VkDescriptorSet m_handle;
  RawRef<Device, VkDevice> m_device;

  DescriptorSet() = delete;

public:
  using Ref = RawRef<DescriptorSet, VkDescriptorSet>;
  explicit DescriptorSet(VkDescriptorSet set, Device &device);

  VkDescriptorSet operator*() const { return m_handle; }
  operator VkDescriptorSet() const { return m_handle; }

  void update(DescriptorSetWrite &write);
};

class DescriptorSetWrite : public VkWriteDescriptorSet {
public:
  DescriptorSetWrite(DescriptorSet &set, uint32_t binding);
};

class DescriptorSetWriteBuffer : public DescriptorSetWrite {
  std::vector<info::DescriptorBuffer> m_buffers;

  void setupBuffers() {
    descriptorCount = static_cast<uint32_t>(m_buffers.size());
    pBufferInfo = m_buffers.data();
  }

public:
  DescriptorSetWriteBuffer(DescriptorSet &set, uint32_t binding);

  DescriptorSetWriteBuffer(DescriptorSet &set, uint32_t binding,
                           UniformBuffer &buffer, uint32_t offset = 0,
                           VkDeviceSize range = VK_WHOLE_SIZE);

  DescriptorSetWriteBuffer &addBuffer(info::DescriptorBuffer &buffer);
};
} // namespace vk
