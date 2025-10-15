#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Input {
public:
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool is_key_pressed(int key);

private:
    static bool keys[1024];
};
