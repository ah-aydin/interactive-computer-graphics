#include <cstdlib>
#include <iostream>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <optional>

const float verticies[] = {
    .5f,  -.5f, 0.0f, 1.f, 0.f, 0.f, // Bottom right
    0.0f, .5f,  0.0f, 0.f, 1.f, 0.f, // Top
    -.5f, -.5f, 0.0f, 0.f, 0.f, 1.f  // Bottom left
};

static const char *vertex_shader_src = "#version 330 core\n"
                                       "layout(location=0) in vec3 pos;\n"
                                       "layout(location=1) in vec3 color;\n"
                                       "out vec3 vColor;\n"
                                       "void main(){\n"
                                       "  gl_Position = vec4(pos, 1.0);\n"
                                       "  vColor = color;\n"
                                       "}\n";
static const char *frag_shader_src = "#version 330 core\n"
                                     "layout(location=0) out vec4 color;\n"
                                     "in vec3 vColor;\n"
                                     "void main(){\n"
                                     "  color = vec4(vColor, 1.0);\n"
                                     "}\n";

void error_callback(int error, const char *descriptor) {
  std::cerr << "Error: " << descriptor << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void size_callback(GLFWwindow *window, int w, int h) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

std::optional<GLuint> create_shader(const char *src, const GLuint shader_type) {
  const GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  GLint compilation_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
  if (compilation_status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar buffer[2048];
    glGetShaderInfoLog(shader, 2048, &log_length, buffer);
    std::cerr << "Error compiling shader: " << buffer << std::endl;
  }
  return shader;
}

std::optional<GLuint> create_shader_program(const char *vertex_src,
                                            const char *frag_src) {
  const std::optional<GLuint> vertex_shader =
      create_shader(vertex_src, GL_VERTEX_SHADER);
  if (!vertex_shader.has_value()) {
    return {};
  }
  const std::optional<GLuint> fragment_shader =
      create_shader(frag_src, GL_FRAGMENT_SHADER);
  if (!fragment_shader.has_value()) {
    return {};
  }

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader.value());
  glAttachShader(program, fragment_shader.value());
  glLinkProgram(program);

  GLint link_status;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar buffer[2048];
    glGetProgramInfoLog(program, 2048, &log_length, buffer);
    std::cerr << "Error linking shader program: " << buffer << std::endl;
    return {};
  }

  return program;
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialze GLFW\n";
    return EXIT_FAILURE;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(640, 480, "01 Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to create GLFWwindow\n";
    return EXIT_FAILURE;
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetWindowSizeCallback(window, size_callback);

  glfwMakeContextCurrent(window);
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return EXIT_FAILURE;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
            << GLAD_VERSION_MINOR(version) << std::endl;

  float r = 0, g = 0, b = 0;
  int r_dir = 1, g_dir = 1, b_dir = 1;

  std::optional<GLuint> maybe_program =
      create_shader_program(vertex_shader_src, frag_shader_src);
  if (!maybe_program.has_value()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GLuint shader_program = maybe_program.value();

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (GLvoid *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (GLvoid *)(sizeof(float) * 3));

  double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double delta = time - last_time;

    r += (10 * delta) * r_dir;
    g += (20 * delta) * g_dir;
    b += (30 * delta) * b_dir;
    if (r > 255) {
      r_dir *= -1;
    }
    if (g > 255) {
      g_dir *= -1;
    }
    if (b > 255) {
      b_dir *= -1;
    }

    glClearColor(r / 255.f, g / 255.f, b / 255.f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glUseProgram(shader_program);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    last_time = time;
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
