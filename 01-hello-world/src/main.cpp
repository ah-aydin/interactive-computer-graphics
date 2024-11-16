#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void checkOpenGLError(const char *stmt, const char *file, int line) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    const char *error;
    switch (err) {
    case GL_INVALID_ENUM:
      error = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "GL_INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      error = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "GL_STACK_UNDERFLOW";
      break;
    case GL_OUT_OF_MEMORY:
      error = "GL_OUT_OF_MEMORY";
      break;
    default:
      error = "Unknown Error";
      break;
    }
    std::cerr << "OpenGL error (" << error << "): " << stmt << " in " << file
              << " at line " << line << std::endl;
  }
}

#ifdef DEBUG
#define GL_CALL(stmt)                                                          \
  stmt;                                                                        \
  checkOpenGLError(#stmt, __FILE__, __LINE__);
#else
#define GL_CALL(stmt) stmt;
#endif

const float verticies[] = {
    // Positions         // Colors
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // Vertex 0: Red
    1.0f,  -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Vertex 1: Green
    1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, // Vertex 2: Blue
    -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 0.0f, // Vertex 3: Yellow
    -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 1.0f, // Vertex 4: Magenta
    1.0f,  -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, // Vertex 5: Cyan
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // Vertex 6: White
    -1.0f, 1.0f,  1.0f,  0.0f, 0.0f, 0.0f  // Vertex 7: Black
};

const unsigned int indicies[] = {
    // Front face
    4, 5, 6, 4, 6, 7,

    // Back face
    0, 1, 2, 0, 2, 3,

    // Left face
    4, 7, 3, 4, 3, 0,

    // Right face
    1, 5, 6, 1, 6, 2,

    // Top face
    7, 6, 2, 7, 2, 3,

    // Bottom face
    4, 5, 1, 4, 1, 0};

static const char *vertex_shader_src = "#version 330 core\n"
                                       "layout(location=0) in vec3 pos;\n"
                                       "layout(location=1) in vec3 color;\n"
                                       "out vec3 vColor;\n"
                                       "uniform mat4 mvp;\n"
                                       "void main(){\n"
                                       "  gl_Position = mvp * vec4(pos, 1.0);\n"
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
  GL_CALL(glViewport(0, 0, width, height));
}

std::optional<GLuint> create_shader(const char *src, const GLuint shader_type) {
  const GLuint shader = glCreateShader(shader_type);
  GL_CALL(glShaderSource(shader, 1, &src, NULL));
  GL_CALL(glCompileShader(shader));

  GLint compilation_status;
  GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status));
  if (compilation_status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar buffer[2048];
    GL_CALL(glGetShaderInfoLog(shader, 2048, &log_length, buffer));
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

  const GLuint program = GL_CALL(glCreateProgram());
  GL_CALL(glAttachShader(program, vertex_shader.value()));
  GL_CALL(glAttachShader(program, fragment_shader.value()));
  GL_CALL(glLinkProgram(program));

  GLint link_status;
  GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &link_status));
  if (link_status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar buffer[2048];
    GL_CALL(glGetProgramInfoLog(program, 2048, &log_length, buffer));
    std::cerr << "Error linking shader program: " << buffer << std::endl;
    return {};
  }

  return program;
}

int main() {
#ifdef DEBUG
  std::cout << "Running in DEBUG mode\n";
#endif
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
  GL_CALL(glViewport(0, 0, width, height));

  GL_CALL(glEnable(GL_DEPTH_TEST));
  GL_CALL(glFrontFace(GL_CCW));

  std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
            << GLAD_VERSION_MINOR(version) << std::endl;

  std::optional<GLuint> maybe_program =
      create_shader_program(vertex_shader_src, frag_shader_src);
  if (!maybe_program.has_value()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GLuint shader_program = maybe_program.value();
  GLuint mvp_position = GL_CALL(glGetUniformLocation(shader_program, "mvp"));

  GLuint vao;
  GL_CALL(glGenVertexArrays(1, &vao));
  GL_CALL(glBindVertexArray(vao));

  GLuint vbo;
  GL_CALL(glGenBuffers(1, &vbo));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies,
                       GL_STATIC_DRAW));
  GL_CALL(glEnableVertexAttribArray(0));
  GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                                (GLvoid *)0));
  GL_CALL(glEnableVertexAttribArray(1));
  GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                                (GLvoid *)(sizeof(float) * 3)));

  GLuint ebo;
  GL_CALL(glGenBuffers(1, &ebo));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
                       GL_STATIC_DRAW));

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
                               glm::vec3(0.0f, 0.0f, 0.0f), // Target
                               glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
  );
  glm::mat4 projection =
      glm::perspective(glm::radians(45.f), 800.f / 680.f, 0.1f, 500.f);

  glm::mat4 base_model = glm::mat4(1.0f);
  base_model = glm::scale(base_model, glm::vec3(.5f, .5f, .5f));
  base_model = glm::translate(base_model, glm::vec3(0.f, 0.f, -4.f));

  float yaw = 0.f, pitch = 0.f, roll = 0.f;
  int yaw_dir = 1, pitch_dir = 1, roll_dir = 1;

  float r = 0, g = 0, b = 0;
  int r_dir = 1, g_dir = 1, b_dir = 1;

  double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double delta = time - last_time;

    r += (10 * delta) * r_dir;
    g += (20 * delta) * g_dir;
    b += (30 * delta) * b_dir;
    if (r > 255 && r < 0) {
      r_dir *= -1;
    }
    if (g > 255 && g < 0) {
      g_dir *= -1;
    }
    if (b > 255 || b < 0) {
      b_dir *= -1;
    }

    GL_CALL(glClearColor(r / 255.f, g / 255.f, b / 255.f, 1));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    yaw += (30 * delta) * yaw_dir;
    pitch += (40 * delta) * pitch_dir;
    roll += (50 * delta) * roll_dir;
    if (yaw > 360 * 20 || yaw < 0) {
      yaw_dir *= -1;
    }
    if (pitch > 360 * 20 || pitch < 0) {
      pitch_dir *= -1;
    }
    if (roll > 360 * 20 || roll < 0) {
      roll_dir *= -1;
    }

    glm::mat4 model =
        glm::rotate(base_model, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 mvp = projection * view * model;

    GL_CALL(glUseProgram(shader_program));
    GL_CALL(glUniformMatrix4fv(mvp_position, 1, GL_FALSE, glm::value_ptr(mvp)));

    GL_CALL(glBindVertexArray(vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));

    glfwSwapBuffers(window);
    glfwPollEvents();

    last_time = time;
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
