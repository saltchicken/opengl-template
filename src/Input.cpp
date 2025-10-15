#include "Input.h"

bool Input::keys[1024] = { false };

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

bool Input::is_key_pressed(int key) {
    if (key >= 0 && key < 1024) {
        return keys[key];
    }
    return false;
}
