#pragma once

#include "pipeline/pipeline.hpp"
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class RenderPass;

namespace info {
class GraphicsPipelineCreate : public VkGraphicsPipelineCreateInfo {
  RawRef<PipelineLayout, VkPipelineLayout> m_layout;
  std::vector<PipelineShaderStageCreate> m_stages;

public:
  GraphicsPipelineCreate()
      : VkGraphicsPipelineCreateInfo{.sType =
                                         VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                     .pNext = nullptr,
                                     .flags = 0,
                                     .stageCount = 0,
                                     .pStages = nullptr,
                                     .pVertexInputState = nullptr,
                                     .pInputAssemblyState = nullptr,
                                     .pTessellationState = nullptr,
                                     .pViewportState = nullptr,
                                     .pRasterizationState = nullptr,
                                     .pMultisampleState = nullptr,
                                     .pDepthStencilState = nullptr,
                                     .pColorBlendState = nullptr,
                                     .pDynamicState = nullptr,
                                     .layout = VK_NULL_HANDLE,
                                     .renderPass = VK_NULL_HANDLE,
                                     .subpass = 0,
                                     .basePipelineHandle = VK_NULL_HANDLE,
                                     .basePipelineIndex = -1},
        m_layout(nullptr) {}

  GraphicsPipelineCreate(PipelineLayout &pipelineLayout, RenderPass &renderPass,
                         std::span<PipelineShaderStageCreate> &stages,
                         PipelineVertexInputStateCreate &vertexInputState,
                         PipelineInputAssemblyStateCreate &inputAssemblyState,
                         PipelineViewportStateCreate &viewportState,
                         PipelineRasterizationStateCreate &rasterizationState,
                         PipelineMultisampleStateCreate &multisampleState,
                         PipelineColorBlendStateCreate &colorBlendState,
                         PipelineDynamicStateCreate &dynamicState,
                         uint32_t subpassIndex = 0);
  auto getLayout() -> PipelineLayout & { return m_layout.value(); }
};

} // namespace info
class GraphicsPipeline : public Pipeline {
public:
  GraphicsPipeline(VkPipeline pipeline, Device &device, PipelineLayout &layout)
      : Pipeline(pipeline, device, layout) {}

  GraphicsPipeline(GraphicsPipeline &&other) noexcept
      : Pipeline(std::move(other)) {}

  static std::optional<GraphicsPipeline>
  create(Device &device, info::GraphicsPipelineCreate createInfo);

  [[nodiscard]] auto bindPoint() const -> VkPipelineBindPoint override {
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
  }
};
} // namespace vk
