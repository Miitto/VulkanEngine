#include "shaderModule.hpp"

#include "util/vk-logger.hpp"

#include "device/device.hpp"

#include <fstream>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
ShaderModule::ShaderModule(VkShaderModule module, Device &device)
    : Handle(module), m_device(device.ref()) {}
namespace info {

auto ShaderModuleCreate::checkCode(const std::vector<char> &code) -> bool {
  if (code.size() % sizeof(uint32_t) != 0) {
    Logger::error("Shader code size is not a multiple of 4 bytes.");
    return false;
  }

  const auto *codePtr = reinterpret_cast<const uint32_t *>(code.data());

  if (codePtr[0] != 0x07230203) {
    Logger::error("Invalid magic number in shader code.");
    return false;
  }

  return true;
}
} // namespace info

auto readFile(const std::string &fileName) -> std::optional<std::vector<char>> {
  std::ifstream file(fileName, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    Logger::error("Failed to open file: {}", fileName);
    return std::nullopt;
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), static_cast<long long>(fileSize));

  file.close();

  return buffer;
}

auto ShaderModule::fromFile(const std::string &fileName, Device &device)
    -> std::optional<ShaderModule> {
  auto code = readFile(fileName);
  if (!code) {
    Logger::error("Failed to read shader file: {}", fileName);
    return std::nullopt;
  }

  return ShaderModule::fromCode(*code, device);
}

auto ShaderModule::fromCode(std::vector<char> &code, Device &device)
    -> std::optional<ShaderModule> {
  info::ShaderModuleCreate createInfo(code);

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    Logger::error("Failed to create shader module");
    return std::nullopt;
  }

  ShaderModule module(shaderModule, device);
  return module;
}

auto ShaderModule::destroy() -> void {
  vkDestroyShaderModule(m_device, m_handle, nullptr);
}
} // namespace vk
