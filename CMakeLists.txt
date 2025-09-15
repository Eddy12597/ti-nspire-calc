cmake_minimum_required(VERSION 3.30)
project(ti_repl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(ti_repl src/main.cpp)
target_include_directories(ti_repl PUBLIC include)

add_library(ti_repl_lib STATIC src/utils.cpp src/value.cpp src/repl.cpp)
target_link_libraries(ti_repl ti_repl_lib)
