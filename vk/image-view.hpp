#pragma once

#include "handle.hpp"
#include "ref.hpp"
#include <optional>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;
class Image;

namespace enums {
class Format;
}
} // namespace vk

namespace vk {
namespace info {
class ImageViewCreate : public VkImageViewCreateInfo {
public:
  ImageViewCreate(Image &image, enums::Format format);

  auto setImage(VkImage img) -> ImageViewCreate & {
    image = img;
    return *this;
  }

  auto setViewType(VkImageViewType view) -> ImageViewCreate & {
    viewType = view;
    return *this;
  }

  auto setFormat(VkFormat fmt) -> ImageViewCreate & {
    format = fmt;
    return *this;
  }

  auto setComponents(VkComponentMapping comps) -> ImageViewCreate & {
    components = comps;
    return *this;
  }

  auto setSubresourceRange(VkImageSubresourceRange subRange)
      -> ImageViewCreate & {
    subresourceRange = subRange;
    return *this;
  }
};

} // namespace info
class ImageView : public Handle<VkImageView> {
  RawRef<Device, VkDevice> m_device;

  ImageView(VkImageView handle, Device &device);

public:
  ImageView(ImageView &&other) noexcept = default;

  static auto create(Device &device, info::ImageViewCreate info)
      -> std::optional<ImageView>;

  auto destroy() -> void override;
};
} // namespace vk
