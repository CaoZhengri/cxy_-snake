cmake_minimum_required(VERSION 3.16)

project(snake VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(snake
    main.cpp
    core/GameLoop.cpp
    core/StateMachine.cpp
    systems/InputSystem.cpp
    systems/RenderSystem.cpp
    systems/TimeSystem.cpp
    gameplay/Snake.cpp
    gameplay/Food.cpp
    persistence/SaveSystem.cpp
)

if(MSVC)
    target_compile_options(snake PRIVATE /W4)
else()
    target_compile_options(snake PRIVATE -Wall -Wextra -Wpedantic)
endif()
