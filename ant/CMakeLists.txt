cmake_minimum_required(VERSION 3.0.0)
project(ant)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.c
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL2::SDL2 SDL2::SDL2main
)
