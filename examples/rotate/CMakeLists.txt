cmake_minimum_required(VERSION 2.8)

project(e_line_judge)

find_package(OpenCV REQUIRED)

add_executable(rotate_frames
    src/rotate_frames.cpp
)

include_directories(rotate_frames
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(rotate_frames
    ${OpenCV_LIBS}
)
