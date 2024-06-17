cmake_minimum_required(VERSION 3.10)
project(LinuxShellProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(shell src/main.cpp src/shell.cpp src/shell.h)
