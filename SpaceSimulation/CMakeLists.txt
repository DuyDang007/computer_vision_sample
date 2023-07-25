# CMakeList.txt : CMake project, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.

include_directories(AFTER "E:/Program\ Files/CUDA_Toolkits/devtool/include")
include_directories(AFTER "E:/Work/opencv/build/include")
link_directories(AFTER "E:/Program\ Files/CUDA_Toolkits/devtool/lib/x64")
link_directories(AFTER "E:/Work/opencv/build/x64/vc15/lib")

find_package(OpenCV PATHS "E:/Work/opencv/build/")
link_libraries(OpenCL)
link_libraries(opencv_world460d)

add_executable (simulation "simulation.cpp" "display_image.cpp")

# TODO: Add tests and install targets if needed.;
