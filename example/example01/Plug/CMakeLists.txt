cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PluginMgr VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PLUGINMGR_SOURCES
	main.cpp
)

add_executable(PluginMgr01 ${PLUGINMGR_SOURCES})
set_target_properties(PluginMgr01 PROPERTIES OUTPUT_NAME PluginMgr)
target_include_directories(PluginMgr01 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include
	)
target_link_libraries(PluginMgr01 gplug)

install(TARGETS PluginMgr01 DESTINATION bin/example01)

if (WIN32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplugin/gplugin_win.xml" DESTINATION bin/example01/gplugin RENAME gplugin.xml)
else()
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplugin/gplugin.xml" DESTINATION bin/example01/gplugin)
endif()

