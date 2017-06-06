cmake_minimum_required(VERSION 3.6)
project(syspro3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c contentServer.c mirrorServer.c mirrorServer.h mirrorInitiator.c pool.h)
add_executable(syspro3 ${SOURCE_FILES})