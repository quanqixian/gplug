cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(CatPlugin VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CATPLUGIN_SOURCES
	CatPlugin.c	
	Cat.c
)

add_library(CatPlugin02 SHARED ${CATPLUGIN_SOURCES})
set_target_properties(CatPlugin02 PROPERTIES OUTPUT_NAME CatPlugin)
target_include_directories(CatPlugin02 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include)

install(TARGETS CatPlugin02 DESTINATION bin/example02/CatPlugin)

