#include <ofyaGl/shader.h>

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace ofyaGl {

GLuint Shader::createShader(const char *src, const GLuint shaderType) {
  const GLuint shader = glCreateShader(shaderType);
  GL_CALL(glShaderSource(shader, 1, &src, NULL));
  GL_CALL(glCompileShader(shader));

  GLint compilation_status;
  GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status));
  if (compilation_status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar buffer[2048];
    GL_CALL(glGetShaderInfoLog(shader, 2048, &log_length, buffer));
    std::cerr << "Error compiling shader: " << buffer << std::endl;
    return 0;
  }
  return shader;
}

Shader Shader::fromSrc(const char *vertSrc, const char *fragSrc) {
  const GLuint vertShaderId = createShader(vertSrc, GL_VERTEX_SHADER);
  if (vertShaderId == 0) {
    return Shader(0);
  }
  const GLuint fragShaderId = createShader(fragSrc, GL_FRAGMENT_SHADER);
  if (fragShaderId == 0) {
    return Shader(0);
  }

  const GLuint program = GL_CALL(glCreateProgram());
  GL_CALL(glAttachShader(program, vertShaderId));
  GL_CALL(glAttachShader(program, fragShaderId));
  GL_CALL(glLinkProgram(program));

  GLint linkStatus;
  GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));
  if (linkStatus != GL_TRUE) {
    GLsizei logLength = 0;
    GLchar buffer[2048];
    GL_CALL(glGetProgramInfoLog(program, 2048, &logLength, buffer));
    std::cerr << "Error linking shader program: " << buffer << std::endl;
    return Shader(0);
  }

  return Shader(program);
}

GLuint Shader::getUniformPosition(const char *uniformName) {
  auto entry = this->uniformToPos.find(uniformName);
  if (entry != this->uniformToPos.end()) {
    return entry->second;
  }
  GLuint uniformPosition =
      GL_CALL(glGetUniformLocation(this->programId, uniformName));
  this->uniformToPos.insert({uniformName, uniformPosition});
  return uniformPosition;
}

void Shader::setUniform(const GLuint uniformPosition,
                        const glm::mat4 mat4) const {
  GL_CALL(
      glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(mat4)));
}

void Shader::setUniform(const char *uniformName, const glm::mat4 mat4) {
  GLuint uniformPosition = getUniformPosition(uniformName);
  GL_CALL(
      glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(mat4)));
}

} // namespace ofyaGl
