cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(gplugin VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(GPLUG_WITH_TESTS "Compile and run test executables" ON)
option(GPLUG_WITH_EXAMPLE "Compile examples" ON)
option(GPLUG_BUILD_SHARED_LIBS "Build lib as a shared library." ON)
option(GPLUG_BUILD_STATIC_LIBS "Build lib as a static library." ON)
option(GPLUG_BUILD_DOXYGEN_DOC "Generate API documentation using doxygen." ON)

add_subdirectory(thirdparty)

#install the includes
add_subdirectory(include)

add_subdirectory(src)

#if(GPLUGIN_WITH_EXAMPLE)
#	add_subdirectory(example)
#endif()

#if(GPLUGIN_WITH_TESTS)
#	enable_testing()
#	include(CTest)
#	add_subdirectory(test)
#endif()

#if(GPLUGIN_BUILD_DOXYGEN_DOC)
#	find_package(Doxygen)
#	if(DOXYGEN_FOUND)
#		add_subdirectory(doxygen)
#	else()
#		message(WARNING "Doxygen not found, will not build API documentation.")
#	endif()
#endif()
