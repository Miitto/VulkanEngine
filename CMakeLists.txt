cmake_minimum_required(VERSION 3.15...4.0)

project(VulkanRenderer VERSION 0.1 DESCRIPTION "Vulkan Renderer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
    renderer.ixx
)

include(FetchContent)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG master)
FetchContent_MakeAvailable(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${glm_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    $<INSTALL_INTERFACE:include/renderer>
)

add_subdirectory(vk)
target_link_libraries(${PROJECT_NAME} PUBLIC vk)
