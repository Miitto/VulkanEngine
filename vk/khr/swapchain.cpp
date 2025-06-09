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
auto SwapchainCreate::setOldSwapchain(khr::Swapchain &swapchain)
    -> SwapchainCreate & {
  oldSwapchain = swapchain;
  return *this;
}
} // namespace info

namespace khr {
Swapchain::Swapchain(VkSwapchainKHR swapchain, Device &device,
                     std::vector<Image> &images,
                     std::vector<ImageView> &imageViews,
                     std::vector<Semaphore> &semaphores, Extent2D extent,
                     enums::Format format)
    : Handle(swapchain), m_device(device.ref()), m_images(std::move(images)),
      m_imageViews(std::move(imageViews)),
      m_imageSemaphores(std::move(semaphores)), m_extent(extent),
      m_format(format) {}

auto Swapchain::destroy() -> void {
  if (m_handle != VK_NULL_HANDLE) {
    for (auto &imageView : m_imageViews) {
      vkDestroyImageView(**m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(**m_device, m_handle, nullptr);
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

  std::vector<Semaphore> semaphores;
  semaphores.reserve(imageCount);
  for (size_t i = 0; i < imageCount; ++i) {
    info::SemaphoreCreate semaphoreCreateInfo;
    auto semaphore = Semaphore::create(device, semaphoreCreateInfo);
    if (!semaphore.has_value()) {
      Logger::error("Failed to create semaphore for swap chain image!");
      vkDestroySwapchainKHR(*device, swapChain, nullptr);
      return std::nullopt;
    }
    semaphores.push_back(std::move(semaphore.value()));
  }

  return Swapchain(swapChain, device, images, imageViews, semaphores,
                   info.imageExtent, info.imageFormat);
}

auto Swapchain::createFramebuffers(RenderPass &renderPass)
    -> std::optional<std::vector<Framebuffer>> {
  std::vector<Framebuffer> framebuffers;
  for (auto &imageView : m_imageViews) {
    info::FramebufferCreate builder(renderPass, m_extent.width,
                                    m_extent.height);
    auto framebufferCreateInfo = builder.addAttachment(imageView);

    auto framebuffer = Framebuffer::create(*m_device, framebufferCreateInfo);
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
      m_device, m_handle, timeout, semaphoreHandle, fenceHandle, &imageIndex);

  auto &sem = m_imageSemaphores[imageIndex];

  return {.state = result, .imageIndex = imageIndex, .semaphore = sem};
}
} // namespace khr
} // namespace vk
