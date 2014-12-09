cmake_minimum_required(VERSION 2.8)

project(e_line_judge)

find_package(OpenCV REQUIRED)

add_executable(e_line_judge
    src/main.cpp
    src/ball_detection.cpp
    src/ball_tracking.cpp   
    src/line_decision.cpp
    src/surface_contact_detection.cpp
    src/user_calibration.cpp
    src/pre_processing.cpp
)

include_directories(e_line_judge
    include/
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(e_line_judge
    ${OpenCV_LIBS}
)
