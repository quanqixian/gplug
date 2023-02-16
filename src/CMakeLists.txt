cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(libgplug VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GPLUG_SOURCES
	gplug.cpp
	tinyxml2.cpp
)

if(GPLUG_BUILD_SHARED_LIBS)
	add_library(gplug_shared SHARED ${GPLUG_SOURCES})
	set_target_properties(gplug_shared PROPERTIES OUTPUT_NAME gplug)

	target_include_directories(gplug_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
	if (UNIX)
		target_link_libraries(gplug_shared ${CMAKE_DL_LIBS})
    endif()

	install(TARGETS gplug_shared)
endif()

if(GPLUG_BUILD_STATIC_LIBS)
	add_library(gplug_static STATIC ${GPLUG_SOURCES})
	target_include_directories(gplug_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)

    #On Windows you should probably give each library a different name, since there is a ".lib" file for both shared and static.
    #But on Linux and Mac you can even give both libraries the same name (e.g. libMyLib.a and libMyLib.so)
    if (WIN32)
        set_target_properties(gplug_static PROPERTIES OUTPUT_NAME gplug_static)
    else()
        set_target_properties(gplug_static PROPERTIES OUTPUT_NAME gplug)
		target_link_libraries(gplug_static ${CMAKE_DL_LIBS})
    endif()

	install(TARGETS gplug_static)
endif()
