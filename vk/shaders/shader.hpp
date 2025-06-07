#pragma once

#include "shaderModule.hpp"

#include <bit>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {

class ShaderStage {
public:
  enum Value {
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
    TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
  };

private:
  Value value;

public:
  ShaderStage() = delete;
  constexpr ShaderStage(Value value) : value(value) {}

  operator VkShaderStageFlagBits() const {
    return std::bit_cast<VkShaderStageFlagBits>(value);
  }
};

class Shader {
  ShaderModule module;
  ShaderStage stage;

public:
  Shader(ShaderModule &module, ShaderStage stage)
      : module(std::move(module)), stage(stage) {}

  Shader(Shader &&other) noexcept
      : module(std::move(other.module)), stage(other.stage) {}

  auto getStage() -> ShaderStage { return stage; };

  auto getModule() -> ShaderModule & { return module; }

  static auto fromCode(std::vector<char> &code, ShaderStage stage,
                       Device &device) -> std::optional<Shader>;
  static auto fromFile(const std::string &fileName, ShaderStage stage,
                       Device &device) -> std::optional<Shader>;
};
} // namespace vk
