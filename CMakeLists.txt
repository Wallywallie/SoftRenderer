cmake_minimum_required(VERSION 3.0.0)

project(renderer VERSION 0.1.0)

#设置exe输出目录
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

# 添加库，包含源文件
add_library(my_library tgaimage.cpp)

add_definitions("-Wall -ggdb -g -pg -O3")

add_executable(renderer main.cpp)
target_link_libraries(renderer PRIVATE my_library)


