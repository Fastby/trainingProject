cmake_minimum_required(VERSION 3.10)

project(Project 
LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(SERVER src/Daemon.cpp headers/Buffer.h headers/ClientSet.h headers/FD_Listener)
add_executable(CLIENT src/Client.cpp headers/Buffer.h headers/FD_Connection)
