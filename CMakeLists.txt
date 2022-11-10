cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 11)

project(sfm_test)
set(SFM_PROJECT_PATH "/e/dense_flow")

# Add dependencies
set(CMAKE_MODULE_PATH
    ${SFM_PROJECT_PATH}/opencv4.5.5)

find_package(OpenCV REQUIRED)

message(STATUS ${OpenCV_LIBS})

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

########################### MAIN PROJECT ############################
# Add source files
add_executable(sfm_test
    src/main.cpp)

# Add linked libraries
target_link_libraries(sfm_test
    ${OpenCV_LIBS} stdc++)
    
#####################################################################

########################### TEST READ FILE ##########################
add_executable(read_test
    src/data_read.cpp)

# Add linked libraries
target_link_libraries(read_test
    stdc++)
#####################################################################
