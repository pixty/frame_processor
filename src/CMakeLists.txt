#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

cmake_minimum_required(VERSION 2.8.4)

PROJECT(FrameProcessor)

include(../3rdParty/dlib/cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
find_package(OpenCV)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})

    ADD_EXECUTABLE(frame_processor frame_processor.cpp)
    TARGET_LINK_LIBRARIES(frame_processor dlib ${OpenCV_LIBS} )
else ()
 	message("No OpenCV cannot build it frame_processor :(")
endif ()
