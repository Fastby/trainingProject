cmake_minimum_required(VERSION 3.10)

project(Project 
LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -pedantic")


add_executable(SERVER src/Daemon.cpp headers/Buffer.h headers/ClientSet.h headers/FD_Listener.h)
add_executable(CLIENT src/Client.cpp headers/Buffer.h headers/FD_Connection.h)

target_include_directories(CLIENT PRIVATE ./headers)
target_include_directories(SERVER PRIVATE ./headers)