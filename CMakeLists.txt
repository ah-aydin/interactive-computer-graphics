cmake_minimum_required(VERSION 3.28)

project(InterativeComputerGraphics)

set(COMMON_LIBS_DIR ${CMAKE_SOURCE_DIR}/common)
set(EXTERNAL_LIBS_DIR ${CMAKE_SOURCE_DIR}/external)

add_subdirectory(${COMMON_LIBS_DIR}/ofyaGl)

add_subdirectory(${EXTERNAL_LIBS_DIR}/glad)
add_subdirectory(${EXTERNAL_LIBS_DIR}/glfw)
add_subdirectory(${EXTERNAL_LIBS_DIR}/glm)

add_subdirectory(01-hello-world)
add_subdirectory(02-obj-loading)
add_subdirectory(03-shading)
