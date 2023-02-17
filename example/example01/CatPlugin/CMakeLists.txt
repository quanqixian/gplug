cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(CatPlugin VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CATPLUGIN_SOURCES
	CatPlugin.cpp	
	Cat.cpp
)

add_library(CatPlugin SHARED ${CATPLUGIN_SOURCES})
target_include_directories(CatPlugin PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include)

install(TARGETS CatPlugin DESTINATION bin/example01/CatPlugin)

