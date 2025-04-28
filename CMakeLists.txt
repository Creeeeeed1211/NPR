cmake_minimum_required(VERSION 3.10)
project(SDL2_Test)

# Set the C++ standard to C++17 for std::filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the SDL2 library
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add the executable
add_executable(SDL2_Test SDL2_Test.cpp)

# Link SDL2 library
target_link_libraries(SDL2_Test ${SDL2_LIBRARIES})

# Link the filesystem library if using GCC
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_libraries(SDL2_Test stdc++fs)
endif()
