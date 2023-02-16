cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(gplug_include VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplug.h" TYPE INCLUDE)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplugin.h" TYPE INCLUDE)
