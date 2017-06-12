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

FIND_PACKAGE(Boost REQUIRED program_options signals thread system)
if (Boost_FOUND)
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
else()
    message(FATAL_ERROR "Boost not found, please install it.")
endif()

include_directories(${CPR_INCLUDE_DIRS})

# add all files
# aux_source_directory(. SRC_LIST)
set(SRC_LIST
    video_streaming/file_video_stream.cpp
    video_streaming/file_video_stream.hpp
    video_streaming/image_window.hpp
    video_streaming/webcam_video_stream.cpp
    video_streaming/webcam_video_stream.hpp
    logger.cpp
    logger.hpp
    main.cpp
    model.cpp
    model.hpp
   )

add_executable(frame_processor ${SRC_LIST})
# set_target_properties(frame_processor PROPERTIES OUTPUT_NAME "fp")
target_link_libraries(frame_processor ${dlib_LIBS} ${OpenCV_LIBS} ${Boost_LIBRARIES} ${CPR_LIBRARIES})

# copy resources function

function(copy_resource)
  cmake_parse_arguments(MY_ARGS "" "SRC;DST" "" ${ARGN})
  add_custom_command(
        TARGET frame_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_PROJECT_DIRECTORY}/resources/${MY_ARGS_SRC}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MY_ARGS_DST})
endfunction(copy_resource)

# copy default config file
# fix me
# copy_resource( SRC config_camera_source.txt DST default.cfg )
