#pragma once

#include "util/vk-logger.hpp"
#include <bit>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vk {
class ShaderStageFlags {
public:
  enum Bits : VkShaderStageFlags {
    None = 0,
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Compute = VK_SHADER_STAGE_COMPUTE_BIT,
    Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
    TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    Raygen = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    AnyHit = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
    ClosestHit = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
    Miss = VK_SHADER_STAGE_MISS_BIT_KHR,
    Intersection = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
    Callable = VK_SHADER_STAGE_CALLABLE_BIT_KHR
  };

private:
  VkShaderStageFlags m_flags;

  static constexpr VkShaderStageFlags allFlags =
      Vertex | Fragment | Compute | Geometry | TessellationControl |
      TessellationEvaluation | Raygen | AnyHit | ClosestHit | Miss |
      Intersection | Callable;

public:
  constexpr ShaderStageFlags() : m_flags(None) {}
  constexpr ShaderStageFlags(VkShaderStageFlags flags) : m_flags(flags) {
    if (flags & ~allFlags) {
      if (std::is_constant_evaluated()) {
        throw std::invalid_argument("Invalid VkBufferUsageFlags provided");
      } else {
        Logger::error(
            "Invalid VkBufferUsageFlags provided: {:b}. Valid Mask: {:b}",
            flags, allFlags);
      }
    }
  }
  constexpr ShaderStageFlags(Bits bits) : m_flags(bits) {}

  constexpr operator VkShaderStageFlags() const { return m_flags; }

  constexpr auto operator|(Bits bits) const -> ShaderStageFlags {
    return (m_flags | bits);
  }

  constexpr auto operator|(const ShaderStageFlags &other) const
      -> ShaderStageFlags {
    return (m_flags | other.m_flags);
  }
};
} // namespace vk

constexpr auto operator|(vk::ShaderStageFlags::Bits bits,
                         vk::ShaderStageFlags flags) {
  return flags | bits;
}
