cmake_minimum_required(VERSION 3.28)
project(01-hello-world VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external_libs/glad)
add_subdirectory(external_libs/glfw)

file(GLOB SRC_FILES src/*.cpp src/*.h src/*.hpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
