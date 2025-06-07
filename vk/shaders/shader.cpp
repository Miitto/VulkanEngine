#include "shader.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
auto Shader::fromFile(const std::string &fileName, ShaderStage stage,
                      Device &device) -> std::optional<Shader> {

  auto module_opt = ShaderModule::fromFile(fileName, device);

  if (!module_opt.has_value())
    return std::nullopt;

  ShaderModule module = std::move(*module_opt);

  Shader shader(module, stage);

  return shader;
}

auto Shader::fromCode(std::vector<char> &code, ShaderStage stage,
                      Device &device) -> std::optional<Shader> {

  auto module_opt = ShaderModule::fromCode(code, device);

  if (!module_opt.has_value())
    return std::nullopt;

  ShaderModule module = std::move(*module_opt);

  Shader shader(module, stage);

  return shader;
}
} // namespace vk
