#include "buffer.hpp"

#include "device/device.hpp"
#include "util/vk-logger.hpp"

#include "descriptors.hpp"
#include "pipeline/layout.hpp"
#include "pipeline/pipeline.hpp"

#include <optional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk {
using Encoder = CommandBuffer::Encoder;

auto CommandBuffer::begin() -> Encoder {
  info::CommandBufferBegin beginInfo{};

  return begin(beginInfo);
}

auto CommandBuffer::begin(info::CommandBufferBegin beginInfo) -> Encoder {
  if (encoder.has_value()) {
    Logger::warn(
        "CommandBuffer already has an encoder, returning existing one.");
    Encoder enc = std::move(**encoder);
    return enc;
  }

  vkBeginCommandBuffer(m_handle, &beginInfo);

  CommandBuffer::Encoder enc(*this);

  encoder = enc.ref();

  return enc;
}

auto Encoder::beginRenderPass(const VkRenderPassBeginInfo info,
                              const VkSubpassContents contents)
    -> Encoder::RenderPass {
  vkCmdBeginRenderPass(**commandBuffer, &info, contents);

  CommandBuffer::Encoder::RenderPass renderPass(*this);

  activeRenderPass = renderPass.ref();

  return renderPass;
}

void Encoder::RenderPass::bindPipeline(const Pipeline &pipeline) {
  if (!*this)
    return;
  m_pipeline = pipeline.ref();
  vkCmdBindPipeline(getCmd(), pipeline.bindPoint(), pipeline);
}

void Encoder::RenderPass::end() {
  if (!*this)
    return;
  vkCmdEndRenderPass(getCmd());
  encoder->value().activeRenderPass = std::nullopt;
  encoder = std::nullopt;
}

Encoder::RenderPass::~RenderPass() {
  if (m_ref.is(this)) {
    end();
  }
}

void Encoder::RenderPass::setViewport(const VkViewport &viewport) {
  if (!*this)
    return;
  vkCmdSetViewport(getCmd(), 0, 1, &viewport);
}

void Encoder::RenderPass::setScissor(const VkRect2D &scissor) {
  if (!*this)
    return;
  vkCmdSetScissor(getCmd(), 0, 1, &scissor);
}

void Encoder::RenderPass::bindVertexBuffer(uint32_t binding,
                                           VertexBuffer &buffer,
                                           VkDeviceSize offset) {
  if (!*this)
    return;
  vkCmdBindVertexBuffers(getCmd(), binding, 1, &*buffer, &offset);
}

void Encoder::RenderPass::bindIndexBuffer(IndexBuffer &buffer,
                                          VkDeviceSize offset) {
  if (!*this)
    return;
  vkCmdBindIndexBuffer(getCmd(), *buffer, offset, buffer.indexType());
}

void Encoder::RenderPass::bindDescriptorSet(
    const DescriptorSet &set, const std::span<uint32_t> dynamicOffsets) {
  if (!*this)
    return;

  if (!m_pipeline.has_value()) {
    Logger::error(
        "No pipeline bound to render pass, cannot bind descriptor set.");
    return;
  }

  if (!m_pipeline.value().has_value()) {
    Logger::error("Pipeline is not valid, cannot bind descriptor set.");
    return;
  }

  auto &pipeline = m_pipeline.value().value();

  VkDescriptorSet rawSet = set;
  VkDescriptorSet *ptrSet = &rawSet;

  VkCommandBuffer rawCommandBuffer = getCmd();

  vkCmdBindDescriptorSets(rawCommandBuffer, pipeline.bindPoint(),
                          pipeline.layout(), 0, 1, ptrSet,
                          dynamicOffsets.size(), dynamicOffsets.data());
}

void Encoder::RenderPass::bindDescriptorSets(
    const std::span<DescriptorSet> &sets, uint32_t firstSet,
    const std::span<uint32_t> dynamicOffsets) {
  if (!*this)
    return;

  if (!m_pipeline.has_value()) {
    Logger::error(
        "No pipeline bound to render pass, cannot bind descriptor sets.");
    return;
  }

  if (!m_pipeline.value().has_value()) {
    Logger::error("Pipeline is not valid, cannot bind descriptor sets.");
    return;
  }

  auto &pipeline = m_pipeline.value().value();

  std::vector<VkDescriptorSet> rawSets(sets.size());
  for (size_t i = 0; i < sets.size(); i++) {
    rawSets[i] = *sets[i];
  }

  vkCmdBindDescriptorSets(getCmd(), pipeline.bindPoint(), pipeline.layout(),
                          firstSet, static_cast<uint32_t>(sets.size()),
                          rawSets.data(), dynamicOffsets.size(),
                          dynamicOffsets.data());
}

void Encoder::RenderPass::bindVertexBuffers(
    uint32_t binding, const std::span<VertexBuffer> &buffers,
    const std::span<VkDeviceSize> &offsets) {
  if (!*this)
    return;
  std::vector<VkBuffer> bufferHandles(buffers.size());
  for (size_t i = 0; i < buffers.size(); i++) {
    bufferHandles[i] = *buffers[i];
  }

  vkCmdBindVertexBuffers(getCmd(), binding,
                         static_cast<uint32_t>(buffers.size()),
                         bufferHandles.data(), offsets.data());
}

void Encoder::RenderPass::draw(uint32_t vertexCount, uint32_t instanceCount,
                               uint32_t firstVertex, uint32_t firstInstance) {
  if (!*this)
    return;
  vkCmdDraw(getCmd(), vertexCount, instanceCount, firstVertex, firstInstance);
}

void Encoder::RenderPass::drawIndexed(uint32_t indexCount,
                                      uint32_t instanceCount,
                                      uint32_t firstIndex, int32_t vertexOffset,
                                      uint32_t firstInstance) {
  if (!*this)
    return;
  vkCmdDrawIndexed(getCmd(), indexCount, instanceCount, firstIndex,
                   vertexOffset, firstInstance);
}

void Encoder::copyBuffer(Buffer &src, Buffer &dst, const VkBufferCopy &region) {
  if (!*this)
    return;
  vkCmdCopyBuffer(**commandBuffer, *src, *dst, 1, &region);
}

void Encoder::copyBuffer(Buffer &src, Buffer &dst,
                         const std::span<vk::BufferCopy> &regions) {
  if (!*this)
    return;
  vkCmdCopyBuffer(**commandBuffer, *src, *dst,
                  static_cast<uint32_t>(regions.size()), regions.data());
}

auto Encoder::end() -> VkResult {
  if (!*this)
    return VK_SUCCESS;

  auto res = vkEndCommandBuffer(**commandBuffer);
  commandBuffer.value()->encoder = std::nullopt;
  commandBuffer = std::nullopt;
  return res;
}

Encoder::~Encoder() {
  if (m_ref.is(this)) {
    end();
  }
}
} // namespace vk
