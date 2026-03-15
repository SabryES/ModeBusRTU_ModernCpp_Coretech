cmake_minimum_required(VERSION 3.10)
project(ModBus_RTU_Library LANGUAGES CXX)

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings for cleaner code
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# Tell CMake where to find your .hpp files (assuming they are in the same directory, or an 'include' folder)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------
# Build Server Executable
# ---------------------------------------------------------
add_executable(Server_App Server_App.cpp)
# Link Windows serial communication library
target_link_libraries(Server_App PRIVATE kernel32)

# ---------------------------------------------------------
# Build Client Executable
# ---------------------------------------------------------
add_executable(Client_App Client_App.cpp)
target_link_libraries(Client_App PRIVATE kernel32)