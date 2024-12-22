# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(TensorConcord LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Define directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# Source files
file(GLOB SOURCES ${SRC_DIR}/*.cpp)

# Define the executable target
add_executable(TensorConcord ${SOURCES})

# Specify the output directory for the executable
set_target_properties(TensorConcord PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Define a target to run the program (Linux only)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_custom_target(run 
        COMMAND ${BIN_DIR}/TensorConcord 
        DEPENDS TensorConcord
    )
endif()

# Custom target to clean everything
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing the entire build directory..."
)