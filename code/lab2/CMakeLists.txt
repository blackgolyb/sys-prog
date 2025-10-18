cmake_minimum_required(VERSION 3.10)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MY_HEADER_FILES
    src/utils.h
    src/buffer.h
    src/consumer.h
    src/producer.h
)

set(MY_SOURCE_FILES
    src/utils.cpp
    src/consumer.cpp
    src/producer.cpp
)


add_executable(${PROJECT_NAME}
    src/main.cpp
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
)


target_include_directories(${PROJECT_NAME}
 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
