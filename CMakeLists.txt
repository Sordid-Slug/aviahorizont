cmake_minimum_required(VERSION 3.0.0)
project(project VERSION 0.1.0)

add_executable(project src/main.c)

target_link_libraries(project PRIVATE glfw GLEW GL m)