cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(DogPlugin VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DOGPLUGIN_SOURCES
	DogPlugin.cpp	
	Dog.cpp
)

add_library(DogPluginTest02 SHARED ${DOGPLUGIN_SOURCES})
set_target_properties(DogPluginTest02 PROPERTIES OUTPUT_NAME DogPlugin)
target_include_directories(DogPluginTest02 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
