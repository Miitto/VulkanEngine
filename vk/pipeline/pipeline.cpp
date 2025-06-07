#include "pipeline.hpp"

#include "device/device.hpp"
#include "pipeline/layout.hpp"
#include "shaders/shader.hpp"

namespace vk {
namespace info {
PipelineShaderStageCreate::PipelineShaderStageCreate(Shader &shader)
    : VkPipelineShaderStageCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .stage = shader.getStage(),
          .module = *shader.getModule(),
          .pName = "main",
          .pSpecializationInfo = nullptr} {}
} // namespace info
  //
Pipeline::Pipeline(VkPipeline pipeline, Device &device, PipelineLayout &layout)
    : Handle<VkPipeline>(pipeline), m_device(device.ref()),
      m_layout(layout.ref()) {}
} // namespace vk
