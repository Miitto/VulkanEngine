#pragma once

#include "handle.hpp"
#include "ref.hpp"

#include "descriptors.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;

namespace info {
class PipelineLayoutCreate : public VkPipelineLayoutCreateInfo {
  std::vector<RawRef<DescriptorSetLayout, VkDescriptorSetLayout>> layouts;
  std::vector<VkDescriptorSetLayout> layoutHandles;

  void setupLayouts() {
    setLayoutCount = static_cast<uint32_t>(layouts.size());
    layoutHandles.resize(setLayoutCount);
    for (size_t i = 0; i < setLayoutCount; ++i) {
      layoutHandles[i] = *layouts[i];
    }
    pSetLayouts = layoutHandles.data();
  }

public:
  PipelineLayoutCreate()
      : VkPipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr} {}

  PipelineLayoutCreate(VkPipelineLayoutCreateInfo &&other)
      : VkPipelineLayoutCreateInfo(other) {}

  auto addSetLayout(DescriptorSetLayout &layout) -> PipelineLayoutCreate & {
    layouts.push_back(layout.ref());
    setupLayouts();
    return *this;
  }
};

} // namespace info

class PipelineLayout : public Handle<VkPipelineLayout>,
                       public RawRefable<PipelineLayout, VkPipelineLayout> {
public:
  RawRef<Device, VkDevice> m_device;

  PipelineLayout(VkPipelineLayout layout, Device &device);

public:
  PipelineLayout(PipelineLayout &&other) noexcept = default;
  auto destroy() -> void override {
    vkDestroyPipelineLayout(m_device, m_handle, nullptr);
  }

  static auto create(Device &device, info::PipelineLayoutCreate info)
      -> std::optional<PipelineLayout>;
};
} // namespace vk
