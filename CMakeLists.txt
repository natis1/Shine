cmake_minimum_required(VERSION 3.13)
project(shine)

set(CMAKE_CXX_STANDARD 17)

include_directories("~/.local/lib64")

find_library(KRPC krpc ~/.local/lib /usr/lib /lib ~/.local/lib64)
find_package(Protobuf REQUIRED)
message("Protobuf Libraries are ${PROTOBUF_LIBRARIES}.")


add_library(kerbal SHARED kerbal/connection.cpp kerbal/connection.h kerbal/math.cpp kerbal/math.h)

target_link_libraries(kerbal "${KRPC}" "${PROTOBUF_LIBRARIES}")

find_package(Curses REQUIRED)
message("Curses Libraries are ${CURSES_LIBRARIES}.")

add_executable(shine shine-curses/main.cpp shine-curses/module_menu.cpp shine-curses/module_menu.h shine-curses/display_manager.cpp shine-curses/display_manager.h shine-curses/kerbal_tools/autopilot.cpp shine-curses/kerbal_tools/autopilot.h)

target_link_libraries(shine "${CURSES_LIBRARIES}" "kerbal")
