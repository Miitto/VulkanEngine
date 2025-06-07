#pragma once
#include "handle.hpp"
#include "ref.hpp"
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
class Device;

namespace info {
class ShaderModuleCreate : public VkShaderModuleCreateInfo {
  std::vector<char> m_code;

  void setupCode(const std::vector<char> &code) {

    if (!checkCode(code)) {
      return;
    }

    m_code = code;
    codeSize = m_code.size();
    pCode = reinterpret_cast<const uint32_t *>(m_code.data());
  }

public:
  static auto checkCode(const std::vector<char> &code) -> bool;

  ShaderModuleCreate(const std::vector<char> code)
      : VkShaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = 0,
            .pCode = nullptr} {
    setupCode(code);
  }

  auto setCode(const std::vector<char> &code) -> ShaderModuleCreate & {
    setupCode(code);

    return *this;
  }

  ShaderModuleCreate(const ShaderModuleCreate &other)
      : VkShaderModuleCreateInfo{other}, m_code(other.m_code) {
    setupCode(m_code);
  }

  ShaderModuleCreate(ShaderModuleCreate &&other) noexcept
      : VkShaderModuleCreateInfo{other}, m_code(std::move(other.m_code)) {
    setupCode(m_code);
    other.setupCode(m_code);
  }
};
} // namespace info

class ShaderModule : public Handle<VkShaderModule> {
  RawRef<Device, VkDevice> m_device;

public:
  ShaderModule(VkShaderModule module, Device &device);

  auto destroy() -> void override;

public:
  static auto fromCode(std::vector<char> &code, Device &device)
      -> std::optional<ShaderModule>;
  static auto fromFile(const std::string &fileName, Device &device)
      -> std::optional<ShaderModule>;
};
} // namespace vk
