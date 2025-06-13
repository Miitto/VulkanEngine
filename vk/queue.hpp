#pragma once

#include "device/physical.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <span>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace vk {
class CommandBuffer;
class Fence;

namespace info {
class Submit : public VkSubmitInfo {
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<VkSemaphore> waitSemaphores;
  std::vector<VkSemaphore> signalSemaphores;
  std::vector<VkPipelineStageFlags> waitDstStageMasks;

  void setupCommandBuffers() {
    commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    pCommandBuffers = commandBuffers.empty() ? nullptr : commandBuffers.data();
  }

  void setupWaitSemaphores() {
    waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    pWaitSemaphores = waitSemaphores.empty() ? nullptr : waitSemaphores.data();
    pWaitDstStageMask =
        waitDstStageMasks.empty() ? nullptr : waitDstStageMasks.data();
  }

  void setupSignalSemaphores() {
    signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    pSignalSemaphores =
        signalSemaphores.empty() ? nullptr : signalSemaphores.data();
  }

public:
  Submit()
      : VkSubmitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                     .pNext = nullptr,
                     .waitSemaphoreCount = 0,
                     .pWaitSemaphores = nullptr,
                     .pWaitDstStageMask = nullptr,
                     .commandBufferCount = 0,
                     .pCommandBuffers = nullptr,
                     .signalSemaphoreCount = 0,
                     .pSignalSemaphores = nullptr} {}

  auto addCommandBuffer(VkCommandBuffer commandBuffer) -> Submit & {
    commandBuffers.push_back(commandBuffer);
    setupCommandBuffers();
    return *this;
  }

  auto addWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage)
      -> Submit & {
    waitSemaphores.push_back(semaphore);
    waitDstStageMasks.push_back(stage);
    setupWaitSemaphores();
    return *this;
  }

  auto addSignalSemaphore(VkSemaphore semaphore) -> Submit & {
    signalSemaphores.push_back(semaphore);
    setupSignalSemaphores();
    return *this;
  }

  Submit(const Submit &other)
      : VkSubmitInfo{other}, commandBuffers(other.commandBuffers),
        waitSemaphores(other.waitSemaphores),
        signalSemaphores(other.signalSemaphores),
        waitDstStageMasks(other.waitDstStageMasks) {
    setupCommandBuffers();
    setupWaitSemaphores();
    setupSignalSemaphores();
  }

  Submit(Submit &&other) noexcept
      : VkSubmitInfo{other}, commandBuffers(std::move(other.commandBuffers)),
        waitSemaphores(std::move(other.waitSemaphores)),
        signalSemaphores(std::move(other.signalSemaphores)),
        waitDstStageMasks(std::move(other.waitDstStageMasks)) {
    setupCommandBuffers();
    setupWaitSemaphores();
    setupSignalSemaphores();
    other.setupCommandBuffers();
    other.setupWaitSemaphores();
    other.setupSignalSemaphores();
  }
};
class Present : public VkPresentInfoKHR {
  std::vector<VkSwapchainKHR> swapchains;
  std::vector<VkSemaphore> waitSemaphores;

  void setupSwapchains() {
    swapchainCount = static_cast<uint32_t>(swapchains.size());
    pSwapchains = swapchains.empty() ? nullptr : swapchains.data();
  }

  void setupWaitSemaphores() {
    waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    pWaitSemaphores = waitSemaphores.empty() ? nullptr : waitSemaphores.data();
  }

public:
  Present()
      : VkPresentInfoKHR{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                         .pNext = nullptr,
                         .waitSemaphoreCount = 0,
                         .pWaitSemaphores = nullptr,
                         .swapchainCount = 0,
                         .pSwapchains = nullptr,
                         .pImageIndices = nullptr,
                         .pResults = nullptr} {}

  auto addSwapchain(VkSwapchainKHR swapchain) -> Present & {
    swapchains.push_back(swapchain);

    setupSwapchains();
    return *this;
  }

  auto setImageIndex(uint32_t &index) -> Present & {
    pImageIndices = &index;
    return *this;
  }

  auto setImageIndices(const std::vector<uint32_t> &indices) -> Present & {
    pImageIndices = indices.data();
    return *this;
  }

  auto addWaitSemaphore(VkSemaphore semaphore) -> Present & {
    waitSemaphores.push_back(semaphore);
    setupWaitSemaphores();
    return *this;
  }

  Present(const Present &other)
      : VkPresentInfoKHR{other}, swapchains(other.swapchains),
        waitSemaphores(other.waitSemaphores) {
    setupSwapchains();
    setupWaitSemaphores();
  }

  Present(Present &&other) noexcept
      : VkPresentInfoKHR{other}, swapchains(std::move(other.swapchains)),
        waitSemaphores(std::move(other.waitSemaphores)) {
    setupSwapchains();
    setupWaitSemaphores();
    other.setupSwapchains();
    other.setupWaitSemaphores();
  }
};

} // namespace info

namespace errors {
class Submit {
public:
  enum Values {
    OutOfHostMemory = VK_ERROR_OUT_OF_HOST_MEMORY,
    OutOfDeviceMemory = VK_ERROR_OUT_OF_DEVICE_MEMORY,
    DeviceLost = VK_ERROR_DEVICE_LOST,
  };

private:
  Values value;

public:
  Submit(Values value) : value(value) {}
  operator VkResult() const { return std::bit_cast<VkResult>(value); }
};

using QueueWait = Submit;
} // namespace errors

class Queue : public Handle<VkQueue> {
protected:
  uint32_t familyIndex;

public:
  Queue(VkQueue queue, int familyIndex)
      : Handle(queue), familyIndex(familyIndex) {}
  Queue(const Queue &other)
      : Handle(other.m_handle), familyIndex(other.familyIndex) {}

  auto submit(vk::info::Submit &submitInfo,
              std::optional<Fence *> fence = std::nullopt)
      -> std::optional<errors::Submit>;

  auto submit(std::span<vk::info::Submit> &submitInfo,
              std::optional<Fence *> fence = std::nullopt)
      -> std::optional<errors::Submit>;

  auto submit(CommandBuffer &cmdBuffer,
              std::optional<Fence *> fence = std::nullopt)
      -> std::optional<errors::Submit>;

  auto waitIdle() -> std::optional<errors::QueueWait> {
    auto res = vkQueueWaitIdle(m_handle);
    if (res != VK_SUCCESS) {
      return std::bit_cast<errors::QueueWait>(res);
    }
    return std::nullopt;
  }

  [[nodiscard]] auto getFamilyIndex() const -> uint32_t { return familyIndex; }
};

class PresentQueue : Queue {
public:
  PresentQueue(VkQueue queue, int familyIndex) : Queue(queue, familyIndex) {}
  PresentQueue(Queue &&other) : Queue(other) {}
  auto present(vk::info::Present &presentInfo) -> VkResult {
    return vkQueuePresentKHR(m_handle, &presentInfo);
  }
};

class QueueFamily {
  PhysicalDevice device;
  VkQueueFamilyProperties family;
  uint32_t index;

public:
  QueueFamily(PhysicalDevice device, VkQueueFamilyProperties family, int index)
      : device(std::move(device)), family(family), index(index) {}

  auto canPresentTo(khr::Surface &surface) const -> bool;

  [[nodiscard]] auto getIndex() const -> uint32_t { return index; }

  [[nodiscard]] auto getFamily() const -> const VkQueueFamilyProperties & {
    return family;
  }

  [[nodiscard]] auto getDevice() const -> PhysicalDevice { return device; }

  [[nodiscard]] auto getQueueCount() const -> uint32_t {
    return family.queueCount;
  }

  [[nodiscard]] auto getQueueFlags() const -> VkQueueFlags {
    return family.queueFlags;
  }

  [[nodiscard]] auto isCompute() const -> bool {
    return family.queueFlags & VK_QUEUE_COMPUTE_BIT;
  }

  [[nodiscard]] auto hasGraphics() const -> bool {
    return family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
  }
};

class QueueFamilies {
  PhysicalDevice m_device;
  std::vector<QueueFamily> m_families;

public:
  QueueFamilies(PhysicalDevice device, std::vector<QueueFamily> &queues)
      : m_device(std::move(device)), m_families(std::move(queues)) {}

  [[nodiscard]] auto size() const -> size_t { return m_families.size(); }

  [[nodiscard]] auto operator[](size_t index) const -> const QueueFamily & {
    return m_families[index];
  }

  [[nodiscard]] auto begin() const -> decltype(m_families.begin()) {
    return m_families.begin();
  }

  [[nodiscard]] auto end() const -> decltype(m_families.end()) {
    return m_families.end();
  }

  auto getGraphics() -> std::optional<QueueFamily>;
  auto getPresent(khr::Surface &surface) -> std::optional<QueueFamily>;
  auto getGraphicsPresent(khr::Surface &surface) -> std::optional<QueueFamily>;
};
} // namespace vk
