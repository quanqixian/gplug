cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(test02 VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(PlugMgr)
add_subdirectory(CatPlugin)
add_subdirectory(DogPlugin)
