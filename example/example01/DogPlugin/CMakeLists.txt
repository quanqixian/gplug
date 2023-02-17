cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(DogPlugin VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DOGPLUGIN_SOURCES
	DogPlugin.cpp	
	Dog.cpp
)

add_library(DogPlugin SHARED ${DOGPLUGIN_SOURCES})
target_include_directories(DogPlugin PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include)

install(TARGETS DogPlugin DESTINATION bin/example01/DogPlugin)
