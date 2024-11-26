#include <ofyaGl/window.h>

#include <cstdlib>
#include <iostream>
#include <ofyaGl/gl.h>

namespace ofyaGl {

Window::Window(int width, int height, const char *title) {

  if (!glfwInit()) {
    std::cerr << "Failed to initialze GLFW\n";
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback([](int error, const char *descriptor) {
    std::cerr << "Error (" << error << "): " << descriptor << std::endl;
  });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(width, height, title, NULL, NULL);

  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to create GLFWwindow\n";
    exit(EXIT_FAILURE);
  }

  glfwSetWindowUserPointer(window, &windowData);

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int, int) {
    WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    GL_CALL(glViewport(0, 0, width, height));
    data.width = width;
    data.height = height;
  });

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::cerr << "Failed to initialize OpenGL context\n";
    exit(EXIT_FAILURE);
  }
  std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
            << GLAD_VERSION_MINOR(version) << std::endl;

  glfwGetFramebufferSize(window, &width, &height);
  GL_CALL(glViewport(0, 0, width, height));
  windowData.width = width;
  windowData.height = height;
}

void Window::terminate() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
} // namespace ofyaGl
