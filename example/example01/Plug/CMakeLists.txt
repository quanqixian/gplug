cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PluginMgr VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PLUGINMGR_SOURCES
	main.cpp
)

add_executable(PluginMgr ${PLUGINMGR_SOURCES})
target_include_directories(PluginMgr PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include
	)
target_link_libraries(PluginMgr gplug)

install(TARGETS PluginMgr DESTINATION bin/example01)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gplugin" DESTINATION bin/example01)

