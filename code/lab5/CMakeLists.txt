cmake_minimum_required(VERSION 3.10)
project(lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# Find Threads package for pthread support
find_package(Threads REQUIRED)

# Manager executable
add_executable(manager
    src/manager.cpp
)

target_include_directories(manager
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(manager
    Threads::Threads
)

# Process 1 executable
add_executable(process1
    src/process1.cpp
)

target_include_directories(process1
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(process1
    Threads::Threads
)

# Process 2 executable
add_executable(process2
    src/process2.cpp
)

target_include_directories(process2
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(process2
    Threads::Threads
)

# Process 3 executable
add_executable(process3
    src/process3.cpp
)

target_include_directories(process3
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(process3
    Threads::Threads
)

# Process 4 executable
add_executable(process4
    src/process4.cpp
)

target_include_directories(process4
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(process4
    Threads::Threads
)
