cmake_minimum_required(VERSION 3.12)

project(opengl_demo C)

set(CMAKE_C_STANDARD 11)

FILE(GLOB SOURCE_FILES src/*.c)

add_executable(opengl_demo ${SOURCE_FILES})

include_directories(./deps/include)

link_directories(./deps/lib)

target_link_libraries(opengl_demo -lglfw3)