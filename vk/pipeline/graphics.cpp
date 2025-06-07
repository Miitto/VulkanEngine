#include "graphics.hpp"

#include "device/device.hpp"
#include "pipeline/layout.hpp"
#include "pipeline/renderPass.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
namespace info {
GraphicsPipelineCreate::GraphicsPipelineCreate(
    PipelineLayout &pipelineLayout, RenderPass &renderPass,
    std::span<PipelineShaderStageCreate> &stages,
    PipelineVertexInputStateCreate &vertexInputState,
    PipelineInputAssemblyStateCreate &inputAssemblyState,
    PipelineViewportStateCreate &viewportState,
    PipelineRasterizationStateCreate &rasterizationState,
    PipelineMultisampleStateCreate &multisampleState,
    PipelineColorBlendStateCreate &colorBlendState,
    PipelineDynamicStateCreate &dynamicState, uint32_t subpassIndex)
    : VkGraphicsPipelineCreateInfo{.sType =
                                       VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                   .pNext = nullptr,
                                   .flags = 0,
                                   .stageCount =
                                       static_cast<uint32_t>(stages.size()),
                                   .pStages = nullptr,
                                   .pVertexInputState = &vertexInputState,
                                   .pInputAssemblyState = &inputAssemblyState,
                                   .pTessellationState = nullptr,
                                   .pViewportState = &viewportState,
                                   .pRasterizationState = &rasterizationState,
                                   .pMultisampleState = &multisampleState,
                                   .pDepthStencilState = nullptr,
                                   .pColorBlendState = &colorBlendState,
                                   .pDynamicState = &dynamicState,
                                   .layout = pipelineLayout,
                                   .renderPass = renderPass,
                                   .subpass = subpassIndex,
                                   .basePipelineHandle = VK_NULL_HANDLE,
                                   .basePipelineIndex = -1},
      m_layout(pipelineLayout.ref()) {
  m_stages.reserve(stages.size());
  for (const auto &stage : stages) {
    m_stages.push_back(stage);
  }

  pStages = m_stages.data();
}

} // namespace info
auto GraphicsPipeline::create(Device &device,
                              vk::info::GraphicsPipelineCreate createInfo)
    -> std::optional<GraphicsPipeline> {
  VkPipeline pipeline;
  auto result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                          &createInfo, nullptr, &pipeline);
  if (result != VK_SUCCESS) {
    return std::nullopt;
  }

  return GraphicsPipeline(pipeline, device, createInfo.getLayout());
}
} // namespace vk
