cmake_minimum_required(VERSION 3.15...4.0)

project(VulkanRenderer VERSION 0.1 DESCRIPTION "Vulkan Renderer" LANGUAGES CXX)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG master)
FetchContent_MakeAvailable(glm)
target_link_libraries(renderer PUBLIC glm::glm)

add_subdirectory(vk)
