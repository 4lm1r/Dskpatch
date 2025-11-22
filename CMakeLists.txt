cmake_minimum_required(VERSION 3.10)
project(DSKPATCH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find required libraries
find_library(NCURSES_LIBRARY NAMES ncurses)
find_library(PANEL_LIBRARY NAMES panel)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/WindowManager.cpp
    src/UIRenderer.cpp
    src/InputHandler.cpp
    src/FileManager.cpp
    src/HexEditor.cpp
)

# Executable
add_executable(dskpatch4-3 ${SOURCES})

# Link libraries
target_link_libraries(dskpatch4-3 ${NCURSES_LIBRARY} ${PANEL_LIBRARY})

# Install target
install(TARGETS dskpatch4-3 DESTINATION bin)
