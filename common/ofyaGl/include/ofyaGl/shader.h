#pragma once

#include <glad/gl.h>
#include <glm/matrix.hpp>
#include <ofyaGl/gl.h>

#include <map>

namespace ofyaGl {

class Shader {
private:
  GLuint programId;
  std::map<const char *, GLuint> uniformToPos;

  Shader(GLuint id) : programId(id) {};

  /**
   * Returns 0 on failure
   */
  static GLuint createShader(const char *src, const GLuint shaderType);

public:
  Shader() = delete;
  Shader(const Shader &) = delete;

  /**
   * Check `Shader::isValid` afterwards.
   */
  static Shader fromSrc(const char *vertSource, const char *fragSource);

  inline void use() { GL_CALL(glUseProgram(programId)) }
  inline void stop_use() { GL_CALL(glUseProgram(0)); }

  GLuint getUniformPosition(const char *uniformName);
  void setUniform(const GLuint uniformPosition, const glm::mat4 mat4) const;
  void setUniform(const char *uniformName, const glm::mat4 mat4);

  inline bool isValid() { return programId != 0; }
};
} // namespace ofyaGl
