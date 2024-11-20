cmake_minimum_required(VERSION 3.28)

project(ofyaGl VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES src/*.cpp src/*.h src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC glad glm)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
