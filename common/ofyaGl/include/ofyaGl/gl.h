#pragma once

void checkOpenGLError(const char *stmt, const char *file, int line);

#ifdef DEBUG
#define GL_CALL(stmt)                                                          \
  stmt;                                                                        \
  checkOpenGLError(#stmt, __FILE__, __LINE__);
#else
#define GL_CALL(stmt) stmt;
#endif
