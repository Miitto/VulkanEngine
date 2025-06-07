add_library(vk)

target_sources(vk PUBLIC
  commands/buffer.cpp
  commands/pool.cpp

  device/device.cpp
  device/memory.cpp
  device/physical.cpp

  khr/surface.cpp
  khr/swapchain.cpp

  pipeline/graphics.cpp
  pipeline/layout.cpp
  pipeline/pipeline.cpp
  pipeline/renderPass.cpp

  shaders/shader.cpp
  shaders/shaderModule.cpp

  sync/fence.cpp
  sync/semaphore.cpp

  util/vk-logger.cpp

  buffers.cpp
  instance.cpp
  offset.cpp
  descriptors.cpp
  framebuffer.cpp
  image-view.cpp
  queue.cpp
  window.cpp
)

target_include_directories(vk PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/vk>
)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(vk PUBLIC glfw Vulkan::Vulkan)
target_compile_definitions(vk PUBLIC GLFW_INCLUDE_VULKAN)

FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.15.3)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(vk PUBLIC spdlog::spdlog_header_only)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET PROJECT_DIR PARENT_PATH RENDERER_DIR)
cmake_path(GET RENDERER_DIR PARENT_PATH ROOT_DIR)
add_compile_definitions(CMAKE_PROJECT_DIR="${ROOT_DIR}")
