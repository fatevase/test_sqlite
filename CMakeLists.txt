cmake_minimum_required(VERSION 3.15)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
project(TestSqliteProject)

# set(CMAKE_CXX_STANDARD 11)  # 设置 C++ 标准

file(GLOB SOURCES src/*.cpp) # 获取 src 目录下的所有源文件
add_executable(main ${SOURCES}) # 将所有源文件添加到 main 可执行文件中

include_directories(inc) # 添加头文件目录
target_link_libraries(main sqlite3) # 添加链接库(target, lib)