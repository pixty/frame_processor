#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#
cmake_minimum_required(VERSION 3.1.2)

# Verbose make
# set( CMAKE_VERBOSE_MAKEFILE on )

PROJECT(FrameProcessor)

# set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
      RelWithDebInfo MinSizeRel." FORCE)
endif()

# c++11
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 11)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
endif()

# output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

# add dlib
find_package(dlib)
if(dlib_FOUND)
    include_directories(${dlib_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "dlib not found, please install it.")
endif()

# add opencv
find_package(OpenCV)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found, please install it.")
endif()

FIND_PACKAGE(Boost REQUIRED program_options)
if (Boost_FOUND)
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
else()
    message(FATAL_ERROR "Boost not found, please install it.")
endif()

# add all files
aux_source_directory(. SRC_LIST)
add_executable(frame_processor ${SRC_LIST})

target_link_libraries(frame_processor ${dlib_LIBS} ${OpenCV_LIBS} ${Boost_LIBRARIES})

