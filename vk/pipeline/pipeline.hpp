#pragma once

#include "handle.hpp"
#include "ref.hpp"

#include "structs/vertexInputAttributeDescription.hpp"
#include "structs/vertexInputBindingDescription.hpp"

#include "vulkan/vulkan_core.h"
#include <span>
#include <vector>

namespace vk {
class Device;
class PipelineLayout;
class Shader;

namespace info {
class PipelineShaderStageCreate : public VkPipelineShaderStageCreateInfo {

public:
  PipelineShaderStageCreate(Shader &shader);
};

class PipelineColorBlendStateCreate
    : public VkPipelineColorBlendStateCreateInfo {
  std::vector<VkPipelineColorBlendAttachmentState> attachments;

  void setupAttachments() {
    attachmentCount = static_cast<uint32_t>(attachments.size());
    pAttachments = attachments.data();
  }

public:
  PipelineColorBlendStateCreate()
      : VkPipelineColorBlendStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 0,
            .pAttachments = nullptr,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}} {}
  auto addAttachment(VkPipelineColorBlendAttachmentState attachment)
      -> PipelineColorBlendStateCreate & {
    attachments.push_back(attachment);
    setupAttachments();
    return *this;
  }

  PipelineColorBlendStateCreate(const PipelineColorBlendStateCreate &o) noexcept
      : VkPipelineColorBlendStateCreateInfo{o}, attachments{o.attachments} {
    setupAttachments();
  }

  PipelineColorBlendStateCreate(PipelineColorBlendStateCreate &&o) noexcept
      : VkPipelineColorBlendStateCreateInfo{o},
        attachments(std::move(o.attachments)) {
    o.setupAttachments();
    setupAttachments();
  }
};

class PipelineDynamicStateCreate : public VkPipelineDynamicStateCreateInfo {
  std::vector<VkDynamicState> dynamicStates;

  void setupDynamicStates() {
    dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    pDynamicStates = dynamicStates.data();
  }

public:
  PipelineDynamicStateCreate()
      : VkPipelineDynamicStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr} {}

  auto addDynamicState(VkDynamicState state) -> PipelineDynamicStateCreate & {
    dynamicStates.push_back(state);
    setupDynamicStates();
    return *this;
  }

  auto addDynamicStates(std::vector<VkDynamicState> states)
      -> PipelineDynamicStateCreate & {
    dynamicStates.insert(dynamicStates.end(), states.begin(), states.end());
    setupDynamicStates();
    return *this;
  }

  auto setDynamicStates(std::vector<VkDynamicState> states)
      -> PipelineDynamicStateCreate & {
    dynamicStates = std::move(states);
    setupDynamicStates();
    return *this;
  }

  PipelineDynamicStateCreate(const PipelineDynamicStateCreate &other)
      : VkPipelineDynamicStateCreateInfo{other},
        dynamicStates(other.dynamicStates) {
    setupDynamicStates();
  }

  PipelineDynamicStateCreate(PipelineDynamicStateCreate &&other) noexcept
      : VkPipelineDynamicStateCreateInfo{other},
        dynamicStates(std::move(other.dynamicStates)) {
    setupDynamicStates();
    other.setupDynamicStates();
  }
};

class PipelineInputAssemblyStateCreate
    : public VkPipelineInputAssemblyStateCreateInfo {

public:
  PipelineInputAssemblyStateCreate()
      : VkPipelineInputAssemblyStateCreateInfo{
            .sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE} {}

  auto setTopology(VkPrimitiveTopology top)
      -> PipelineInputAssemblyStateCreate & {
    topology = top;
    return *this;
  }

  auto setPrimitiveRestartEnable(VkBool32 enable)
      -> PipelineInputAssemblyStateCreate & {
    primitiveRestartEnable = enable;
    return *this;
  }
};

class PipelineMultisampleStateCreate
    : public VkPipelineMultisampleStateCreateInfo {
public:
  PipelineMultisampleStateCreate()
      : VkPipelineMultisampleStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE} {}
};

class PipelineRasterizationStateCreate
    : public VkPipelineRasterizationStateCreateInfo {

public:
  PipelineRasterizationStateCreate()
      : VkPipelineRasterizationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f} {}

  PipelineRasterizationStateCreate &setPolygonMode(VkPolygonMode mode) {
    polygonMode = mode;
    return *this;
  }

  PipelineRasterizationStateCreate &setLineWidth(float width) {
    lineWidth = width;
    return *this;
  }

  PipelineRasterizationStateCreate &setCullMode(VkCullModeFlags mode,
                                                VkFrontFace front) {
    cullMode = mode;
    frontFace = front;
    return *this;
  }
};

class PipelineVertexInputStateCreate
    : public VkPipelineVertexInputStateCreateInfo {

  std::vector<vk::VertexInputBindingDescription> m_vertexBindingDescriptions;
  std::vector<vk::VertexInputAttributeDescription>
      m_vertexAttributeDescriptions;

  void setupBindingDescriptions() {
    vertexBindingDescriptionCount =
        static_cast<uint32_t>(m_vertexBindingDescriptions.size());
    pVertexBindingDescriptions = m_vertexBindingDescriptions.data();
  }

  void setupAttributeDescriptions() {
    vertexAttributeDescriptionCount =
        static_cast<uint32_t>(m_vertexAttributeDescriptions.size());
    pVertexAttributeDescriptions = m_vertexAttributeDescriptions.data();
  }

public:
  PipelineVertexInputStateCreate()
      : VkPipelineVertexInputStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr} {}

  PipelineVertexInputStateCreate &addBindingDescription(
      const vk::VertexInputBindingDescription &bindingDescription) {
    m_vertexBindingDescriptions.push_back(bindingDescription);
    setupBindingDescriptions();
    return *this;
  }

  PipelineVertexInputStateCreate &addBindingDescriptions(
      const std::span<vk::VertexInputBindingDescription> &bindingDescriptions) {
    m_vertexBindingDescriptions.insert(m_vertexBindingDescriptions.end(),
                                       bindingDescriptions.begin(),
                                       bindingDescriptions.end());
    setupBindingDescriptions();
    return *this;
  }

  PipelineVertexInputStateCreate &addAttributeDescription(
      const vk::VertexInputAttributeDescription &attributeDescription) {
    m_vertexAttributeDescriptions.push_back(attributeDescription);
    setupAttributeDescriptions();
    return *this;
  }

  PipelineVertexInputStateCreate &
  addAttributeDescriptions(const std::span<vk::VertexInputAttributeDescription>
                               &attributeDescriptions) {
    m_vertexAttributeDescriptions.insert(m_vertexAttributeDescriptions.end(),
                                         attributeDescriptions.begin(),
                                         attributeDescriptions.end());
    setupAttributeDescriptions();
    return *this;
  }

  PipelineVertexInputStateCreate(const PipelineVertexInputStateCreate &other)
      : VkPipelineVertexInputStateCreateInfo{other},
        m_vertexBindingDescriptions(other.m_vertexBindingDescriptions),
        m_vertexAttributeDescriptions(other.m_vertexAttributeDescriptions) {
    setupBindingDescriptions();
    setupAttributeDescriptions();
  }

  PipelineVertexInputStateCreate(
      PipelineVertexInputStateCreate &&other) noexcept
      : VkPipelineVertexInputStateCreateInfo{other},
        m_vertexBindingDescriptions(
            std::move(other.m_vertexBindingDescriptions)),
        m_vertexAttributeDescriptions(
            std::move(other.m_vertexAttributeDescriptions)) {
    setupBindingDescriptions();
    setupAttributeDescriptions();
    other.setupBindingDescriptions();
    other.setupAttributeDescriptions();
  }
};

class PipelineViewportStateCreate : public VkPipelineViewportStateCreateInfo {
  std::vector<VkViewport> viewports;
  std::vector<VkRect2D> scissors;

  void setupViewports() {
    viewportCount = static_cast<uint32_t>(viewports.size());
    pViewports = viewports.data();
  }

  void setupScissors() {
    scissorCount = static_cast<uint32_t>(scissors.size());
    pScissors = scissors.data();
  }

public:
  PipelineViewportStateCreate()
      : VkPipelineViewportStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 0,
            .pViewports = nullptr,
            .scissorCount = 0,
            .pScissors = nullptr} {}

  auto addViewport(const VkViewport &viewport)
      -> PipelineViewportStateCreate & {
    viewports.push_back(viewport);

    setupViewports();

    return *this;
  }

  auto addScissor(const VkRect2D &scissor) -> PipelineViewportStateCreate & {
    scissors.push_back(scissor);

    setupScissors();

    return *this;
  }

  PipelineViewportStateCreate(const PipelineViewportStateCreate &other)
      : VkPipelineViewportStateCreateInfo{other}, viewports(other.viewports),
        scissors(other.scissors) {
    setupViewports();
    setupScissors();
  }

  PipelineViewportStateCreate(PipelineViewportStateCreate &&other) noexcept
      : VkPipelineViewportStateCreateInfo{other},
        viewports(std::move(other.viewports)),
        scissors(std::move(other.scissors)) {
    setupViewports();
    setupScissors();
    other.setupViewports();
    other.setupScissors();
  }
};

} // namespace info
class PipelineColorBlendAttachmentState
    : public VkPipelineColorBlendAttachmentState {

public:
  PipelineColorBlendAttachmentState()
      : VkPipelineColorBlendAttachmentState{
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

        } {}
};

class Pipeline : public Handle<VkPipeline>,
                 public RawRefable<Pipeline, VkPipeline> {
protected:
  RawRef<Device, VkDevice> m_device;
  RawRef<PipelineLayout, VkPipelineLayout> m_layout;

public:
  Pipeline(VkPipeline pipeline, Device &device, PipelineLayout &pipelineLayout);
  Pipeline(Pipeline &&other) noexcept = default;

  auto destroy() -> void override {
    vkDestroyPipeline(m_device, m_handle, nullptr);
  }

  [[nodiscard]] virtual auto bindPoint() const -> VkPipelineBindPoint = 0;

  auto layout() -> PipelineLayout & { return m_layout.value(); }
};
} // namespace vk
