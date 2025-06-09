#pragma once

#include "handle.hpp"
#include "ref.hpp"

#include "buffers.hpp"
#include "device/device.hpp"
#include "device/memory.hpp"
#include "structs/clearValue.hpp"

#include "util/vk-logger.hpp"

#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <optional>
#include <span>
#include <utility>

namespace vk {
class Pipeline;
class DescriptorSet;
class Buffer;
class VertexBuffer;
class IndexBuffer;

namespace info {
class CommandBufferBegin : public VkCommandBufferBeginInfo {

public:
  CommandBufferBegin()
      : VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr} {}

  auto oneTime() -> CommandBufferBegin & {
    flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    return *this;
  }

  auto renderPassContinue() -> CommandBufferBegin & {
    flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    return *this;
  }

  auto simultaneousUse() -> CommandBufferBegin & {
    flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    return *this;
  }
};

class RenderPassBegin : public VkRenderPassBeginInfo {
  std::vector<ClearValue> clearValues;

  void setupClearValues() {
    clearValueCount = static_cast<uint32_t>(clearValues.size());
    pClearValues = clearValues.data();
  }

public:
  RenderPassBegin(VkRenderPass renderPass, VkFramebuffer framebuffer,
                  VkRect2D renderArea)
      : VkRenderPassBeginInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = renderPass,
            .framebuffer = framebuffer,
            .renderArea = renderArea,
            .clearValueCount = 0,
            .pClearValues = nullptr,
        } {}

  auto addClearValue(const ClearValue &clearValue) -> RenderPassBegin & {
    clearValues.push_back(clearValue);
    setupClearValues();
    return *this;
  }

  RenderPassBegin(const RenderPassBegin &o) noexcept
      : VkRenderPassBeginInfo{o}, clearValues(o.clearValues) {
    setupClearValues();
  }

  RenderPassBegin(RenderPassBegin &&o) noexcept
      : VkRenderPassBeginInfo{o}, clearValues(std::move(o.clearValues)) {
    o.setupClearValues();
    setupClearValues();
  }
};

} // namespace info

class CommandBuffer : public Handle<VkCommandBuffer>,
                      public RawRefable<CommandBuffer, VkCommandBuffer> {

public:
  class Encoder : public Refable<Encoder> {
    std::optional<Reference<CommandBuffer>> commandBuffer;

  public:
    Encoder(CommandBuffer &commandBuffer)
        : Refable(), commandBuffer(commandBuffer.ref()),
          activeRenderPass(nullptr) {}

    Encoder(const Encoder &) = delete;
    auto operator=(const Encoder &) -> Encoder & = delete;
    Encoder(Encoder &&o) noexcept
        : Refable(std::move(o)), commandBuffer(std::move(o.commandBuffer)),
          activeRenderPass(std::move(o.activeRenderPass)) {}

    operator bool() const { return commandBuffer.has_value(); }

    class RenderPass : public Refable<RenderPass> {
      std::optional<Reference<Encoder>> encoder;
      std::optional<RawRef<Pipeline, VkPipeline>> m_pipeline;

      [[nodiscard]] inline auto getCmd() const -> CommandBuffer & {
        return encoder->value().commandBuffer.value().value();
      }

    public:
      RenderPass() = delete;
      RenderPass(Encoder &encoder) : Refable(), encoder(encoder.ref()) {}
      RenderPass(const RenderPass &) = delete;
      auto operator=(const RenderPass &) -> RenderPass & = delete;
      RenderPass(RenderPass &&o) noexcept
          : Refable(std::move(o)), encoder(std::move(o.encoder)) {}
      operator bool() const { return encoder.has_value(); }

      void bindPipeline(const vk::Pipeline &pipeline);

      void setViewport(const VkViewport &viewport);
      void setScissor(const VkRect2D &scissor);

      void bindVertexBuffer(uint32_t binding, VertexBuffer &buffer,
                            VkDeviceSize bufferOffset = 0);

      void bindVertexBuffers(uint32_t binding,
                             const std::span<VertexBuffer> &buffers,
                             const std::span<VkDeviceSize> &offsets);

      void bindIndexBuffer(
          IndexBuffer &buffer, VkDeviceSize offset = 0,
          VkIndexType indexType = VkIndexType::VK_INDEX_TYPE_UINT32);

      void bindDescriptorSet(const DescriptorSet &set,
                             const std::span<uint32_t> dynamicOffsets = {});
      void bindDescriptorSets(const std::span<DescriptorSet> &sets,
                              uint32_t firstSet = 0,
                              const std::span<uint32_t> dynamicOffsets = {});

      void draw(uint32_t vertexCount, uint32_t instanceCount = 1,
                uint32_t firstVertex = 0, uint32_t firstInstance = 0);

      void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                       uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                       uint32_t firstInstance = 0);

      void end();
      ~RenderPass();
    };

    std::optional<Reference<RenderPass>> activeRenderPass;

    auto beginRenderPass(const VkRenderPassBeginInfo info,
                         const VkSubpassContents contents =
                             VK_SUBPASS_CONTENTS_INLINE) -> RenderPass;

    void copyBuffer(Buffer &src, Buffer &dst, const VkBufferCopy &region);
    void copyBuffer(Buffer &src, Buffer &dst,
                    const std::span<BufferCopy> &regions);

    struct TemporaryStaging {
      Buffer buf;
      DeviceMemory memory;
    };

    template <typename T>
    auto writeBufferWithStaging(std::span<T> &data, Buffer &dst,
                                Offset offset = Offset(0))
        -> std::optional<CommandBuffer::Encoder::TemporaryStaging> {
      if (!dst.canCopyTo()) {
        Logger::error("Buffer is not a transfer destination");
        return std::nullopt;
      }

      VkDeviceSize size = data.size() * sizeof(T);
      assert(size > 0);
      assert(dst.size() >= size + offset);

      auto &device = *dst.getDevice();

      auto stagingBufInfo =
          info::BufferCreate(Size(size), enums::BufferUsage::TransferSrc);
      auto stagingBuf_opt = device.createBuffer(stagingBufInfo);
      if (!stagingBuf_opt.has_value()) {
        Logger::error("Failed to create staging buffer");
        return std::nullopt;
      }
      auto &stagingBuffer = stagingBuf_opt.value();

      auto stagingMemory_opt = device.allocateMemory(
          stagingBuffer, enums::MemoryPropertyFlags::HostVisible |
                             enums::MemoryPropertyFlags::HostCoherent);
      if (!stagingMemory_opt.has_value()) {
        Logger::error("Failed to allocate staging buffer memory");
        return std::nullopt;
      }
      auto &stagingMemory = stagingMemory_opt.value();

      auto stagingBufBindRes = stagingBuffer.bind(stagingMemory);
      if (stagingBufBindRes.has_value()) {
        Logger::error("Failed to bind staging buffer memory: {}",
                      stagingBufBindRes.value());
        return std::nullopt;
      }

      {
        auto mapping_opt = stagingMemory.map();
        if (!mapping_opt.has_value()) {
          Logger::error("Failed to map staging buffer");
          return std::nullopt;
        }
        auto &mapping = mapping_opt.value();

        mapping.write(data);
      }

      copyBuffer(
          stagingBuffer, dst,
          VkBufferCopy{.srcOffset = 0, .dstOffset = offset, .size = size});

      CommandBuffer::Encoder::TemporaryStaging staging{
          .buf = std::move(stagingBuffer),
          .memory = std::move(stagingMemory),
      };

      std::optional<CommandBuffer::Encoder::TemporaryStaging> stagingOpt =
          std::make_optional(std::move(staging));

      return stagingOpt;
    }

    auto end() -> VkResult;

    ~Encoder();
  };

private:
  std::optional<Reference<Encoder>> encoder = std::nullopt;

public:
  CommandBuffer(const CommandBuffer &o)
      : Handle(o.m_handle), encoder(o.encoder) {}
  CommandBuffer(VkCommandBuffer commandBuffer) : Handle(commandBuffer) {}

  auto begin() -> Encoder;
  auto begin(info::CommandBufferBegin info) -> Encoder;

  void reset(VkCommandBufferResetFlags flags = 0) {
    vkResetCommandBuffer(m_handle, flags);
  }
};
} // namespace vk
