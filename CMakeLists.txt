cmake_minimum_required(VERSION 3.12)
project(opengl_demo C)

set(CMAKE_C_STANDARD 11)

include_directories(./deps/include)
link_directories(./deps/lib)

add_executable(opengl_demo main.c glad.c glutil.c)
target_link_libraries(opengl_demo -lglfw3)