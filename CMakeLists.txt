cmake_minimum_required(VERSION 3.10)
project(SimpleDatabase C)

# Specify where to look for .h (header) files
include_directories(${PROJECT_SOURCE_DIR}/Headers)

# Gather all .c files in Source/ folder
file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/Source/*.c")

# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})
