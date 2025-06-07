#pragma once

#include <vulkan/vulkan_core.h>

namespace vk {
template <typename T> class Handle {
protected:
  T m_handle;

public:
  Handle() = delete;
  Handle(const Handle &) = delete;
  auto operator=(const Handle &) -> Handle & = delete;

  Handle(T handle) : m_handle(handle) {}
  Handle(Handle &&other) noexcept : m_handle(other.m_handle) {
    other.m_handle = nullptr;
  }

  operator T() const { return m_handle; }
  auto operator*() const -> const T & { return m_handle; }

  [[nodiscard]] auto isValid() const -> bool {
    return m_handle != VK_NULL_HANDLE;
  }
  explicit operator bool() const { return isValid(); }

  virtual auto destroy() -> void {}

  virtual ~Handle() {
    if (m_handle != VK_NULL_HANDLE) {
      destroy();
      m_handle = VK_NULL_HANDLE;
    }
  }
};
} // namespace vk
