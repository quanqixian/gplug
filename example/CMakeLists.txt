cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Examples VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXAMPLES
    example01
    example02
)

foreach(example ${EXAMPLES})
	add_subdirectory(${example})
endforeach()
