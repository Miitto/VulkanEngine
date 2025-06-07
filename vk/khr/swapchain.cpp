#include "swapchain.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"
#include "framebuffer.hpp"
#include "image-view.hpp"
#include "image.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {

namespace info {
auto SwapchainCreate::setOldSwapchain(Swapchain &swapchain)
    -> SwapchainCreate & {
  oldSwapchain = swapchain;
  return *this;
}
} // namespace info

Swapchain::Swapchain(VkSwapchainKHR swapchain, Device &device,
                     std::vector<Image> &images,
                     std::vector<ImageView> &imageViews, Extent2D extent,
                     enums::Format format)
    : Handle(swapchain), device(device.ref()), images(std::move(images)),
      imageViews(std::move(imageViews)), extent(extent), format(format) {}

auto Swapchain::destroy() -> void {
  if (m_handle != VK_NULL_HANDLE) {
    for (auto &imageView : imageViews) {
      vkDestroyImageView(**device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(**device, m_handle, nullptr);
  }
}

auto Swapchain::create(Device &device, info::SwapchainCreate info)
    -> std::optional<Swapchain> {
  VkSwapchainKHR swapChain;
  if (vkCreateSwapchainKHR(*device, &info, nullptr, &swapChain) != VK_SUCCESS) {
    Logger::error("Failed to create swap chain");
    return std::nullopt;
  }

  uint32_t imageCount;
  vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, nullptr);
  std::vector<VkImage> image_handles(imageCount);
  vkGetSwapchainImagesKHR(*device, swapChain, &imageCount,
                          image_handles.data());

  std::vector<Image> images;
  images.reserve(imageCount);
  for (auto &image_handle : image_handles) {
    images.push_back(Image::fromHandle(image_handle));
  }

  std::vector<ImageView> imageViews;
  imageViews.reserve(imageCount);

  for (size_t i = 0; i < imageCount; ++i) {
    auto createInfo = info::ImageViewCreate(images[i], info.imageFormat);
    auto imageView = ImageView::create(device, createInfo);
    if (!imageView.has_value()) {
      Logger::error("Failed to create image view for swap chain image!");
      vkDestroySwapchainKHR(*device, swapChain, nullptr);
      return std::nullopt;
    }
    imageViews.push_back(std::move(imageView.value()));
  }
  return Swapchain(swapChain, device, images, imageViews, info.imageExtent,
                   info.imageFormat);
}

auto Swapchain::createFramebuffers(RenderPass &renderPass)
    -> std::optional<std::vector<Framebuffer>> {
  std::vector<Framebuffer> framebuffers;
  for (auto &imageView : imageViews) {
    info::FramebufferCreate builder(renderPass, extent.width, extent.height);
    auto framebufferCreateInfo = builder.addAttachment(imageView);

    auto framebuffer = Framebuffer::create(*device, framebufferCreateInfo);
    if (!framebuffer.has_value()) {
      Logger::error("Failed to create framebuffer!");
      return std::nullopt;
    }
    framebuffers.push_back(std::move(framebuffer.value()));
  }

  return framebuffers;
}

auto Swapchain::getNextImage(std::optional<Semaphore *> semaphore,
                             std::optional<Fence *> fence, uint64_t timeout)
    -> SwapchainImageState {
  uint32_t imageIndex;

  VkFence fenceHandle = fence.has_value() ? **fence : VK_NULL_HANDLE;
  VkSemaphore semaphoreHandle =
      semaphore.has_value() ? **semaphore : VK_NULL_HANDLE;

  VkResult result = vkAcquireNextImageKHR(
      **device, m_handle, timeout, semaphoreHandle, fenceHandle, &imageIndex);
  return {.state = result, .imageIndex = imageIndex};
}
} // namespace vk
