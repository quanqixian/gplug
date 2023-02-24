cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PluginMgrTest02 VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PLUGINMGR_SOURCES
	main.cpp
)

add_executable(test02 ${PLUGINMGR_SOURCES})
set_target_properties(test02 PROPERTIES OUTPUT_NAME test02)
target_include_directories(test02 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include
	)
target_link_libraries(test02 gplug gtest)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gplugin/
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gplugin)

add_test(NAME test02 COMMAND test02)

