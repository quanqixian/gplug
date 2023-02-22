cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PluginMgrTest01 VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PLUGINMGR_SOURCES
	main.cpp
)

add_executable(test01 ${PLUGINMGR_SOURCES})
set_target_properties(test01 PROPERTIES OUTPUT_NAME test01)
target_include_directories(test01 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../../../include
	)
target_link_libraries(test01 gplug)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gplugin/
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gplugin)

add_test(NAME test01 COMMAND test01)

