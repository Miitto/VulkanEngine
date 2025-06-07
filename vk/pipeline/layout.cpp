#include "layout.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
PipelineLayout::PipelineLayout(VkPipelineLayout layout, Device &device)
    : Handle(layout), m_device(device.ref()) {}

auto PipelineLayout::create(Device &device, vk::info::PipelineLayoutCreate info)
    -> std::optional<PipelineLayout> {
  VkPipelineLayout layout;
  if (vkCreatePipelineLayout(*device, &info, nullptr, &layout) != VK_SUCCESS) {
    Logger::error("Failed to create pipeline layout");
    return std::nullopt;
  }

  PipelineLayout pipelineLayout(layout, device.ref());

  return pipelineLayout;
}
} // namespace vk
