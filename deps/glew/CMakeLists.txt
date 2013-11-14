cmake_minimum_required(VERSION 2.8)
project(glew)

include_directories(include)
add_library(${PROJECT_NAME} src/glew.c)