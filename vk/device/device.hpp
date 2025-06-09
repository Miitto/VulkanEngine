#pragma once

#include "buffers.hpp"
#include "device/physical.hpp"

#include "queue.hpp"
#include "ref.hpp"
#include <optional>
#include <span>
#include <vulkan/vulkan_core.h>

namespace vk {
namespace khr {
class Swapchain;
}
class Queue;
class CommandPool;
class Semaphore;
class Fence;
class Buffer;
class DeviceMemory;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;

class DescriptorSetLayout;
class DescriptorPool;

namespace info {
class DescriptorSetLayoutCreate;
class DescriptorPoolCreate;
class SwapchainCreate;
class CommandPoolCreate;
} // namespace info
} // namespace vk

namespace vk {
namespace info {
class DeviceQueueCreate : public VkDeviceQueueCreateInfo {
  std::vector<float> m_priorities;

  void setupPriorities() {
    queueCount = static_cast<uint32_t>(m_priorities.size());
    pQueuePriorities = m_priorities.empty() ? nullptr : m_priorities.data();
  }

public:
  DeviceQueueCreate(QueueFamily &family, float priority = 1.0f)
      : DeviceQueueCreate(family.getIndex(), priority) {}
  DeviceQueueCreate(uint32_t queueIndex, float priority = 1.0f)
      : DeviceQueueCreate(queueIndex, std::vector<float>{priority}) {}

  DeviceQueueCreate(uint32_t queueIndex, std::vector<float> priorities)
      : VkDeviceQueueCreateInfo{.sType =
                                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                .pNext = nullptr,
                                .flags = 0,
                                .queueFamilyIndex = queueIndex,
                                .queueCount = 0,
                                .pQueuePriorities = nullptr},
        m_priorities(std::move(priorities)) {
    setupPriorities();
  }

  auto setQueuePriorities(std::vector<float> priorities)
      -> DeviceQueueCreate & {
    this->m_priorities = std::move(priorities);
    setupPriorities();
    return *this;
  }

  auto addQueue(float priority) -> DeviceQueueCreate & {
    m_priorities.push_back(priority);
    setupPriorities();

    return *this;
  }

  DeviceQueueCreate(const DeviceQueueCreate &other)
      : VkDeviceQueueCreateInfo{other}, m_priorities(other.m_priorities) {
    setupPriorities();
  }

  DeviceQueueCreate(DeviceQueueCreate &&other) noexcept
      : VkDeviceQueueCreateInfo{other},
        m_priorities(std::move(other.m_priorities)) {
    setupPriorities();
    other.setupPriorities();
  }
};

class DeviceCreate : public VkDeviceCreateInfo {
  PhysicalDeviceFeatures m_features{};
  std::vector<vk::info::DeviceQueueCreate> m_queueCreateInfos{};
  std::vector<char const *> m_extensions{};

  void setupQueues() {
    queueCreateInfoCount = static_cast<uint32_t>(m_queueCreateInfos.size());
    pQueueCreateInfos = m_queueCreateInfos.data();
  }

  void setupExtensions() {
    enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
    ppEnabledExtensionNames = m_extensions.data();
  }

public:
  DeviceCreate()
      : VkDeviceCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                           .pNext = nullptr,
                           .flags = 0,
                           .queueCreateInfoCount = 0,
                           .pQueueCreateInfos = nullptr,
                           .enabledLayerCount = 0,
                           .ppEnabledLayerNames = nullptr,
                           .enabledExtensionCount = 0,
                           .ppEnabledExtensionNames = nullptr,
                           .pEnabledFeatures = nullptr} {}

  DeviceCreate(const PhysicalDeviceFeatures &features) : DeviceCreate() {
    m_features = features;
    pEnabledFeatures = &m_features;
  }

  auto setPhysicalDeviceFeatures(const PhysicalDeviceFeatures &features)
      -> DeviceCreate & {
    m_features = features;
    pEnabledFeatures = &m_features;

    return *this;
  }

  template <typename... Args>
    requires requires {
      !std::same_as<typename std::tuple_element_t<0, std::tuple<Args...>>::type,
                    vk::info::DeviceQueueCreate> &&
          !std::same_as<
              typename std::tuple_element_t<0, std::tuple<Args...>>::type,
              vk::info::DeviceQueueCreate &> &&
          std::constructible_from<DeviceQueueCreate, Args...>;
    }
  auto addQueue(Args &&...args) -> DeviceCreate & {
    DeviceQueueCreate queueCreateInfo(std::forward<Args>(args)...);
    return addQueue(queueCreateInfo);
  }

  auto addQueue(const DeviceQueueCreate &queueCreateInfo) -> DeviceCreate & {
    auto queueIdx = queueCreateInfo.queueFamilyIndex;

    for (auto &info : m_queueCreateInfos) {
      if (info.queueFamilyIndex == queueIdx) {
        return *this;
      }
    }

    m_queueCreateInfos.push_back(queueCreateInfo);

    setupQueues();
    return *this;
  }

  auto enableExtension(const char *extension) -> DeviceCreate & {
    m_extensions.push_back(extension);

    setupExtensions();

    return *this;
  }

  auto enableExtensions(const std::vector<char const *> &extensions)
      -> DeviceCreate & {
    m_extensions.insert(m_extensions.end(), extensions.begin(),
                        extensions.end());

    setupExtensions();

    return *this;
  }

  DeviceCreate(const DeviceCreate &other)
      : VkDeviceCreateInfo{other}, m_features(other.m_features),
        m_queueCreateInfos(other.m_queueCreateInfos),
        m_extensions(other.m_extensions) {
    setupQueues();
    setupExtensions();
  }

  DeviceCreate(DeviceCreate &&other) noexcept
      : VkDeviceCreateInfo{other}, m_features(other.m_features),
        m_queueCreateInfos(std::move(other.m_queueCreateInfos)),
        m_extensions(std::move(other.m_extensions)) {
    pEnabledFeatures = &m_features;
    other.pEnabledFeatures = nullptr;
    setupQueues();
    setupExtensions();
    other.setupQueues();
    other.setupExtensions();
  }
};

} // namespace info
class Device : public RawRefable<Device, VkDevice>, public Handle<VkDevice> {
  PhysicalDevice m_physicalDevice;

public:
  Device(VkDevice device, PhysicalDevice &physicalDevice)
      : RawRefable(), Handle(device), m_physicalDevice(physicalDevice) {}

  void destroy() override {
    waitIdle();
    vkDestroyDevice(m_handle, nullptr);
  }

  static auto create(PhysicalDevice &physicalDevice,
                     vk::info::DeviceCreate &createInfo) noexcept
      -> std::optional<Device>;

  auto getPhysical() -> PhysicalDevice & { return m_physicalDevice; }

  auto getQueue(QueueFamily &family, uint32_t queueIndex)
      -> std::optional<Queue>;
  auto getQueue(int32_t queueFamilyIndex, uint32_t queueIndex)
      -> std::optional<Queue>;

  auto waitIdle() -> VkResult { return vkDeviceWaitIdle(m_handle); }

  auto createCommandPool(vk::info::CommandPoolCreate &info)
      -> std::optional<CommandPool>;
  auto createSwapchain(vk::info::SwapchainCreate &info)
      -> std::optional<khr::Swapchain>;
  auto createSemaphore() -> std::optional<Semaphore>;
  auto createFence(bool createSignaled = false) -> std::optional<Fence>;

  auto createBuffer(vk::info::BufferCreate &info) -> std::optional<Buffer>;
  auto createVertexBuffer(vk::info::VertexBufferCreate &info)
      -> std::optional<VertexBuffer>;
  auto createIndexBuffer(vk::info::IndexBufferCreate &info)
      -> std::optional<IndexBuffer>;
  auto createUniformBuffer(vk::info::UniformBufferCreate &info)
      -> std::optional<UniformBuffer>;

  auto allocateMemory(Buffer &buffer, enums::MemoryPropertyFlags properties)
      -> std::optional<DeviceMemory>;

  auto allocateMemory(std::span<Buffer *> buffers,
                      enums::MemoryPropertyFlags properties)
      -> std::optional<DeviceMemory>;

  auto allocateMemory(MemoryRequirements memReqs,
                      enums::MemoryPropertyFlags porperties)
      -> std::optional<DeviceMemory>;

  void bindBufferMemory(Buffer &buffer, DeviceMemory &memory,
                        uint32_t offset = 0);

  auto
  createDescriptorSetLayout(vk::info::DescriptorSetLayoutCreate &createInfo)
      -> std::optional<vk::DescriptorSetLayout>;
  auto createDescriptorPool(info::DescriptorPoolCreate &createInfo)
      -> std::optional<DescriptorPool>;
};
} // namespace vk
