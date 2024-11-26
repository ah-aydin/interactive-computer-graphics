#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include <cstdlib>
#include <iostream>

#include <ofyaGl/gl.h>
#include <ofyaGl/obj.h>
#include <ofyaGl/shader.h>

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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Expected an obj file input\n";
    return EXIT_FAILURE;
  }

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
  GLFWwindow *window = glfwCreateWindow(640, 480, "03 Shading", NULL, NULL);
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
  GL_CALL(glClearColor(0.2, 0.2, 0.2, 0.2));

  GL_CALL(glEnable(GL_DEPTH_TEST));
  GL_CALL(glFrontFace(GL_CCW));

  std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
            << GLAD_VERSION_MINOR(version) << std::endl;

  ofyaGl::Shader shader = ofyaGl::Shader::fromFile("02.vert", "02.frag");
  if (!shader.isValid()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Load object
  auto objData = ofyaGl::loadObjDataFromFile(argv[1]);
  if (!objData.has_value()) {
    std::cerr << "Failed to load object data\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_FAILURE;
  }

  const float *vertexData =
      reinterpret_cast<const float *>(objData->verts.data());
  const unsigned int *indicies = objData->indicies.data();
  const unsigned int indexCount = objData->indicies.size();

  // Create OpenGl buffers
  GLuint vao;
  GL_CALL(glGenVertexArrays(1, &vao));
  GL_CALL(glBindVertexArray(vao));

  GLuint vbo;
  GL_CALL(glGenBuffers(1, &vbo));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(ofyaGl::Vertex) * objData->verts.size(),
                       vertexData, GL_STATIC_DRAW));
  GL_CALL(glEnableVertexAttribArray(0));
  GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                sizeof(ofyaGl::Vertex), (GLvoid *)0));
  GL_CALL(glEnableVertexAttribArray(1));
  GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                sizeof(ofyaGl::Vertex),
                                (GLvoid *)(sizeof(float) * 3)));
  GL_CALL(glEnableVertexAttribArray(2));
  GL_CALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                                sizeof(ofyaGl::Vertex),
                                (GLvoid *)(sizeof(float) * 6)));

  GLuint ebo;
  GL_CALL(glGenBuffers(1, &ebo));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(unsigned int) * objData->indicies.size(),
                       indicies, GL_STATIC_DRAW));

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
                               glm::vec3(0.0f, 0.0f, 0.0f), // Target
                               glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
  );
  glm::mat4 projection =
      glm::perspective(glm::radians(90.f), 800.f / 680.f, 0.1f, 500.f);

  glm::mat4 base_model = glm::mat4(1.0f);
  base_model = glm::scale(base_model, glm::vec3(.1f, .1f, .1f));
  base_model = glm::translate(base_model, glm::vec3(0.f, 0.f, -15.f));

  float yaw = 0.f, pitch = 0.f, roll = 0.f;
  int yaw_dir = 1, pitch_dir = 1, roll_dir = 1;

  double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double delta = time - last_time;

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

    shader.use();
    shader.setUniform("mvp", mvp);

    // TODO draw model
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();

    last_time = time;
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
