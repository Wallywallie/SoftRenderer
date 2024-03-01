cmake_minimum_required(VERSION 3.20.0)

project(renderer VERSION 0.1.0)

#设置exe输出目录
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(SOURCES
    src/model.cpp
    src/tgaimage.cpp
)

# 添加库，包含源文件
add_library(my_library ${SOURCES})

add_definitions("-Wall -ggdb -g -pg -O0")

add_executable(renderer src/main.cpp)
target_link_libraries(renderer PRIVATE my_library)


