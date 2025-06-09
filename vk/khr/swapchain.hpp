#pragma once

#include "enums/format.hpp"
#include "enums/sharing-mode.hpp"
#include "handle.hpp"
#include "image-view.hpp"
#include "image.hpp"
#include "queue.hpp"
#include "ref.hpp"
#include "structs/extent2d.hpp"
#include "surface.hpp"
#include "sync/fence.hpp"
#include "sync/semaphore.hpp"

#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;
class RenderPass;
class Framebuffer;

} // namespace vk

namespace vk {
namespace khr {
class Swapchain;
}

namespace info {
class SwapchainCreate : public VkSwapchainCreateInfoKHR {
  khr::SurfaceAttributes &swapChainSupport;
  std::vector<uint32_t> m_queueFamilyIndices;

public:
  SwapchainCreate(khr::SurfaceAttributes &swapChainSupport,
                  khr::Surface &surface, bool share = false)
      : VkSwapchainCreateInfoKHR{.sType =
                                     VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                 .pNext = nullptr,
                                 .flags = 0,
                                 .surface = *surface,
                                 .minImageCount = swapChainSupport.capabilities
                                                      .minImageCount,
                                 .imageFormat =
                                     swapChainSupport.formats[0].format,
                                 .imageColorSpace =
                                     swapChainSupport.formats[0].colorSpace,
                                 .imageExtent = swapChainSupport.capabilities
                                                    .currentExtent,
                                 .imageArrayLayers = 1,
                                 .imageUsage =
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                 .imageSharingMode =
                                     share ? VK_SHARING_MODE_CONCURRENT
                                           : VK_SHARING_MODE_EXCLUSIVE,
                                 .queueFamilyIndexCount = 0,
                                 .pQueueFamilyIndices = nullptr,
                                 .preTransform = swapChainSupport.capabilities
                                                     .currentTransform,
                                 .compositeAlpha =
                                     VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                 .presentMode =
                                     swapChainSupport.presentModes[0],
                                 .clipped = VK_TRUE,
                                 .oldSwapchain = VK_NULL_HANDLE},
        swapChainSupport(swapChainSupport) {}

  auto setImageCount(uint32_t count) -> SwapchainCreate & {
    if (count < swapChainSupport.capabilities.minImageCount ||
        count > swapChainSupport.capabilities.maxImageCount) {
      return *this;
    }
    minImageCount = count;
    return *this;
  }

  auto setImageFormat(enums::Format format) -> SwapchainCreate & {
    for (const auto &availableFormat : swapChainSupport.formats) {
      if (availableFormat.format == format) {
        imageFormat = format;
        return *this;
      }
    }

    return *this;
  }

  auto setImageColorSpace(VkColorSpaceKHR colorSpace) -> SwapchainCreate & {
    for (const auto &availableFormat : swapChainSupport.formats) {
      if (availableFormat.colorSpace == colorSpace) {
        imageColorSpace = colorSpace;
        return *this;
      }
    }

    return *this;
  }

  auto setImageExtent(Extent2D extent) -> SwapchainCreate & {
    if (extent.width == 0 || extent.height == 0) {
      return *this;
    }

    if (extent.width < swapChainSupport.capabilities.minImageExtent.width ||
        extent.width > swapChainSupport.capabilities.maxImageExtent.width ||
        extent.height < swapChainSupport.capabilities.minImageExtent.height ||
        extent.height > swapChainSupport.capabilities.maxImageExtent.height) {
    }

    imageExtent = extent;
    return *this;
  }

  auto setImageArrayLayers(uint32_t layers) -> SwapchainCreate & {
    imageArrayLayers = layers;
    return *this;
  }

  auto setPresentMode(VkPresentModeKHR present_mode) -> SwapchainCreate & {
    for (const auto &availablePresentMode : swapChainSupport.presentModes) {
      if (availablePresentMode == present_mode) {
        presentMode = present_mode;
        return *this;
      }
    }

    return *this;
  }

  auto setClipped(VkBool32 clip) -> SwapchainCreate & {
    clipped = clip;
    return *this;
  }

  auto setOldSwapchain(khr::Swapchain &swapchain) -> SwapchainCreate &;

  auto setImageSharingMode(enums::SharingMode sharingMode)
      -> SwapchainCreate & {
    imageSharingMode = sharingMode;
    return *this;
  }

  template <typename... Args>
  auto setQueueFamilyIndices(Args &&...args) -> SwapchainCreate &
    requires(std::conjunction_v<std::is_same<QueueFamily, Args>...> ||
             std::conjunction_v<std::is_same<QueueFamily &, Args>...>)
  {
    return setQueueFamilyIndices(args.getIndex()...);
  }

  template <typename... Args>
  auto setQueueFamilyIndices(Args &&...args) -> SwapchainCreate &
    requires(std::conjunction_v<std::is_same<uint32_t, Args>...>)
  {
    m_queueFamilyIndices.clear();

    for (auto &index : {args...}) {
      if (std::find(m_queueFamilyIndices.begin(), m_queueFamilyIndices.end(),
                    index) == m_queueFamilyIndices.end()) {
        m_queueFamilyIndices.push_back(index);
      }
    }

    queueFamilyIndexCount = static_cast<uint32_t>(m_queueFamilyIndices.size());
    pQueueFamilyIndices = m_queueFamilyIndices.data();
    return *this;
  }

  auto setQueueFamilyIndices(std::vector<uint32_t> &indices)
      -> SwapchainCreate & {
    queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
    m_queueFamilyIndices = std::move(indices);
    pQueueFamilyIndices = m_queueFamilyIndices.data();
    return *this;
  }
};

} // namespace info
namespace khr {
class Swapchain : public Handle<VkSwapchainKHR> {
  RawRef<Device, VkDevice> device;
  std::vector<Image> images;
  std::vector<ImageView> imageViews;
  Extent2D extent;
  enums::Format format;

public:
  Swapchain(VkSwapchainKHR swapchain, Device &device,
            std::vector<Image> &images, std::vector<ImageView> &imageViews,
            Extent2D extent, enums::Format format);

  auto destroy() -> void override;

  Swapchain(Swapchain &&o) noexcept = default;

  auto getImages() -> std::vector<Image> & { return images; }
  auto getImageViews() -> std::vector<ImageView> & { return imageViews; }

public:
  static auto create(Device &device, vk::info::SwapchainCreate info)
      -> std::optional<Swapchain>;
  auto getExtent() -> Extent2D & { return extent; }
  auto getFormat() -> enums::Format & { return format; }

  auto createFramebuffers(RenderPass &renderPass)
      -> std::optional<std::vector<Framebuffer>>;

  struct SwapchainImageState {
    VkResult state;
    uint32_t imageIndex;
  };

  auto getNextImage(std::optional<Semaphore *> semaphore = std::nullopt,
                    std::optional<Fence *> fence = std::nullopt,
                    uint64_t timeout = UINT64_MAX) -> SwapchainImageState;
};
} // namespace khr
} // namespace vk
