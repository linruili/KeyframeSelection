cmake_minimum_required(VERSION 2.8)
project(KeyframeSelection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (WIN32)
    set(OpenCV_DIR "D:\\Research\\opencv-2.4.11\\mingw-build\\install")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
endif (WIN32)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(tracker)
FILE(GLOB_RECURSE sourcefiles "tracker/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCE_FILES main.cpp tools.h constant.h keyframe_init.h keyframe_selection_update.h)

add_executable(KeyframeSelection ${SOURCE_FILES}  ${sourcefiles} )

# add libs you need
# set(OpenCV_LIBS opencv_core opencv_highgui opencv_nonfree)

# linking
target_link_libraries(KeyframeSelection ${OpenCV_LIBS})

