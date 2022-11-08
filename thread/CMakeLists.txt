cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
project(thread)

set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")

#ADD_COMPILE_OPTIONS(-pthread)
# 这样加才行
set(CMAKE_C_FLAGS "-pthread ${CMAKE_C_FLAGS}")

#add_executable(${PROJECT_NAME} main.c)
add_executable(main main.c)

add_subdirectory(mutex)