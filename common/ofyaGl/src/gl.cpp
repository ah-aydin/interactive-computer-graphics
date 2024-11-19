#include <ofyaGl/gl.h>

#include <glad/gl.h>
#include <iostream>

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
