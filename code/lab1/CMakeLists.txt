cmake_minimum_required(VERSION 3.10)
project(main LANGUAGES C)

set(MY_HEADER_FILES
    src/base.h
)

set(MY_SOURCE_FILES
    src/base.c
)

add_executable(${PROJECT_NAME}
    src/main.c
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
)

add_executable(son
    src/son.c
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME}
 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
