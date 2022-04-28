cmake_minimum_required(VERSION 3.12)
project(chip8-emulator)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
  src/main.cpp
  src/chip8.cpp)

find_package(Curses REQUIRED)

add_executable(c8 ${SOURCE})

target_link_libraries(c8 ${CURSES_LIBRARIES})

install(TARGETS c8)
