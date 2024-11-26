#pragma once

#include <glad/gl.h>

#include <GLFW/glfw3.h>

namespace ofyaGl {
class Window {
private:
  GLFWwindow *window;

  struct WindowData {
    int width;
    int height;
    // TODO introduce someway to specify an event function
  };

  WindowData windowData;

public:
  Window() = delete;
  Window(const Window &) = delete;

  Window(int widht, int height, const char *title);

  inline bool shouldClose() { return glfwWindowShouldClose(window); }
  inline void swapBuffers() { return glfwSwapBuffers(window); }
  inline void pollEvents() { return glfwPollEvents(); }

  void terminate();
};
} // namespace ofyaGl
