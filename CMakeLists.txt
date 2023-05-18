cmake_minimum_required(VERSION 3.15)

project(TestSqliteProject)

add_executable(main src/Main.cpp)

target_link_libraries(main sqlite3)