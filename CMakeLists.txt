cmake_minimum_required(VERSION 2.8)
project(KeyframeSelection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CAFFE_INCLUDE /home/download/caffe-master/include/)
set(CAFFE_BUILD_INCLUDE /home/download/caffe-master/build/src/)
set(CUDA_INCLUDE /usr/local/cuda-7.5/targets/x86_64-linux/include/)
set(CAFFE_LIBRARY /home/download/caffe-master/build/lib/)

if (WIN32)
    set(OpenCV_DIR "D:\\Research\\opencv-2.4.11\\mingw-build\\install")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
endif (WIN32)

find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS 'Found OpenCV: ' ${OpenCV_INCLUDE_DIRS})
    add_definitions(-DUSE_OPENCV)
endif ()
include_directories(${CAFFE_INCLUDE} ${CAFFE_BUILD_INCLUDE} ${CUDA_INCLUDE} ${OpenCV_INCLUDE_DIRS})
include_directories(tracker)
link_directories(${CAFFE_LIBRARY})

FILE(GLOB_RECURSE sourcefiles "tracker/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCE_FILES main.cpp tools.h constant.h keyframe_init.h keyframe_selection_update.h run_rcnn_procedure.h Myserver.cpp Myserver.h classification.h landmark_identification.h LCS.h)

add_executable(KeyframeSelection ${SOURCE_FILES}  ${sourcefiles} )

# add libs you need
# set(OpenCV_LIBS opencv_core opencv_highgui opencv_nonfree)

target_compile_options(KeyframeSelection PRIVATE -pthread -fPIC)

# linking
target_link_libraries(KeyframeSelection glog gflags protobuf caffe
        boost_system boost_filesystem m hdf5_hl hdf5 ${OpenCV_LIBS})

