#include "glm/ext/matrix_float3x3.hpp"
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
#include <ofyaGl/window.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Expected an obj file input\n";
    return EXIT_FAILURE;
  }

  ofyaGl::Window window(640, 480, "03 Shading");

  GL_CALL(glClearColor(0.2, 0.2, 0.2, 0.2));

  GL_CALL(glEnable(GL_DEPTH_TEST));
  GL_CALL(glFrontFace(GL_CCW));

  ofyaGl::Shader shader = ofyaGl::Shader::fromFile("03.vert", "03.frag");
  if (!shader.isValid()) {
    window.terminate();
    return EXIT_FAILURE;
  }

  // Load object
  auto objData = ofyaGl::loadObjDataFromFile(argv[1]);
  if (!objData.has_value()) {
    std::cerr << "Failed to load object data\n";
    window.terminate();
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
  base_model = glm::translate(base_model, glm::vec3(0.f, 0.f, -2.f));
  base_model = glm::scale(base_model, glm::vec3(.1f, .1f, .1f));

  float yaw = 0.f, pitch = 0.f, roll = 0.f;
  int yaw_dir = 1, pitch_dir = 1, roll_dir = 1;

  double last_time = glfwGetTime();
  while (!window.shouldClose()) {
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
    glm::mat4 mv = view * model; // Model View
    glm::mat3 mv_n =
        glm::transpose(glm::inverse(glm::mat3(mv))); // Model View for normal
    glm::mat4 mvp = projection * mv;                 // Modal View Projection

    shader.use();
    shader.setUniform("mvp", mvp);
    shader.setUniform("mv_n", mv_n);

    // TODO draw model
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    window.swapBuffers();
    window.pollEvents();

    last_time = time;
  }

  window.terminate();

  return EXIT_SUCCESS;
}
