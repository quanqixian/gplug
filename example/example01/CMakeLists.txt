cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(example01 VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(CatPlugin)
add_subdirectory(DogPlugin)
add_subdirectory(Plug)
