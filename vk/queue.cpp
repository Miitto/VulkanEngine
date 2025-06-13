#include "queue.hpp"

#include "device/physical.hpp"
#include "khr/surface.hpp"
#include "sync/fence.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

#include "commands/buffer.hpp"

namespace vk {
auto Queue::submit(vk::info::Submit &submitInfo, std::optional<Fence *> fence)
    -> std::optional<errors::Submit> {

  VkFence vkFence = fence.has_value() ? **fence : VK_NULL_HANDLE;

  VkResult res = vkQueueSubmit(m_handle, 1, &submitInfo, vkFence);
  if (res != VK_SUCCESS) {
    return std::bit_cast<errors::Submit>(res);
  }
  return std::nullopt;
}

auto Queue::submit(std::span<vk::info::Submit> &submitInfos,
                   std::optional<Fence *> fence)
    -> std::optional<errors::Submit> {
  if (submitInfos.empty()) {
    return std::nullopt;
  }

  std::vector<VkSubmitInfo> vkSubmitInfos(submitInfos.size());
  for (size_t i = 0; i < submitInfos.size(); ++i) {
    vkSubmitInfos[i] = submitInfos[i];
  }

  VkFence vkFence = fence.has_value() ? **fence : VK_NULL_HANDLE;

  VkResult res =
      vkQueueSubmit(m_handle, static_cast<uint32_t>(vkSubmitInfos.size()),
                    vkSubmitInfos.data(), vkFence);
  if (res != VK_SUCCESS) {
    return std::bit_cast<errors::Submit>(res);
  }

  return std::nullopt;
}

auto Queue::submit(CommandBuffer &cmdBuffer, std::optional<Fence *> fence)
    -> std::optional<errors::Submit> {
  vk::info::Submit submitInfo;
  submitInfo.addCommandBuffer(cmdBuffer);
  return submit(submitInfo, fence);
}

auto QueueFamily::canPresentTo(khr::Surface &surface) const -> bool {
  VkBool32 presentSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
  return presentSupport;
}

auto QueueFamilies::getGraphics() -> std::optional<QueueFamily> {
  auto queue = std::ranges::find_if(m_families, [](const QueueFamily &family) {
    return family.hasGraphics();
  });

  if (queue != m_families.end()) {
    return std::make_optional(*queue);
  }

  return std::nullopt;
}

auto QueueFamilies::getPresent(khr::Surface &surface)
    -> std::optional<QueueFamily> {
  auto queue =
      std::ranges::find_if(m_families, [&surface](const QueueFamily &family) {
        return family.canPresentTo(surface);
      });

  if (queue != m_families.end()) {
    return std::make_optional(*queue);
  }

  return std::nullopt;
}

auto QueueFamilies::getGraphicsPresent(khr::Surface &surface)
    -> std::optional<QueueFamily> {
  auto queue =
      std::ranges::find_if(m_families, [&surface](const QueueFamily &family) {
        return family.hasGraphics() && family.canPresentTo(surface);
      });

  if (queue != m_families.end()) {
    return std::make_optional(*queue);
  }

  return std::nullopt;
}
} // namespace vk
