cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PluginMgr VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PLUGINMGR_SOURCES
	main.cpp
)

add_executable(PluginMgr02 ${PLUGINMGR_SOURCES})
set_target_properties(PluginMgr02 PROPERTIES OUTPUT_NAME PluginMgr)
target_include_directories(PluginMgr02 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../interface/ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include
	)
target_link_libraries(PluginMgr02 gplug)
set_target_properties(PluginMgr02 PROPERTIES INSTALL_RPATH "$ORIGIN;../../${CMAKE_INSTALL_LIBDIR}")

install(TARGETS PluginMgr02 DESTINATION bin/example02)

if (WIN32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplugin/gplugin_win.xml" DESTINATION bin/example02/gplugin RENAME gplugin.xml)
else()
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gplugin/gplugin.xml" DESTINATION bin/example02/gplugin)
endif()
