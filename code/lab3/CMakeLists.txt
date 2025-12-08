cmake_minimum_required(VERSION 3.10)
project(lab3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

# Source files
set(SOURCES
    src/main.c
)

# Executable
add_executable(main ${SOURCES})

# Link with required libraries
target_link_libraries(main)
