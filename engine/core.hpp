#pragma once

#include <GLFW/glfw3.h>

namespace engine {
auto init() -> bool;
void pollEvents();

void terminate();
} // namespace engine
