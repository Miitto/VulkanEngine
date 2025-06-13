#include "image-view.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"
#include "enums/format.hpp"
#include "image.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {

namespace info {
ImageViewCreate::ImageViewCreate(Image &image, Format format)
    : VkImageViewCreateInfo{
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .image = image,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = format,
          .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                         .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                         .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                         .a = VK_COMPONENT_SWIZZLE_IDENTITY},
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}} {}
} // namespace info

ImageView::ImageView(VkImageView handle, Device &device)
    : Handle(handle), m_device(device.ref()) {}

auto ImageView::create(Device &device, info::ImageViewCreate info)
    -> std::optional<ImageView> {
  VkImageView handle;
  if (vkCreateImageView(device, &info, nullptr, &handle) != VK_SUCCESS) {
    Logger::error("Failed to create image view");
    return std::nullopt;
  }
  return ImageView(handle, device);
}

auto ImageView::destroy() -> void {
  vkDestroyImageView(m_device, m_handle, nullptr);
}
} // namespace vk
