cmake_minimum_required(VERSION 3.16)

project(snake VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(snake
    src/main.cpp
    src/initial.cpp
    src/draw.cpp
    src/draw2.cpp
    src/input.cpp
    src/logic.cpp
    src/save.cpp
)

if(MSVC)
    target_compile_options(snake PRIVATE /W4)
else()
    target_compile_options(snake PRIVATE -Wall -Wextra -Wpedantic)
endif()
