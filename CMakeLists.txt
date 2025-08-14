cmake_minimum_required(VERSION 3.12...3.26)
project(tinyrenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main2.cpp tgaimage.cpp model.cpp)