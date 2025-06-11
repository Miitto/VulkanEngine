#include "buffer-usage.hpp"

namespace vk::enums {
constexpr const BufferUsage BufferUsage::None = BufferUsage(0),
                            BufferUsage::Vertex =
                                BufferUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
                            BufferUsage::Index =
                                BufferUsage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
                            BufferUsage::Uniform =
                                BufferUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT),
                            BufferUsage::Storage =
                                BufferUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
                            BufferUsage::TransferSrc =
                                BufferUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
                            BufferUsage::TransferDst =
                                BufferUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}
