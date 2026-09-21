cmake_minimum_required(VERSION 3.8)
project(camera_ui_pkg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)

find_package(OpenCV REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


add_executable(camera_node
    src/camera_node/camera_node.cpp
)

target_include_directories(camera_node PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

ament_target_dependencies(camera_node
    rclcpp
    sensor_msgs
    std_msgs
    cv_bridge
)

target_link_libraries(camera_node
    ${OpenCV_LIBRARIES}
)


add_executable(ui_node
    src/ui_node/ui_node.cpp
    ui/camera_ui.ui
)

target_include_directories(ui_node PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

ament_target_dependencies(ui_node
    rclcpp
    sensor_msgs
    std_msgs
    cv_bridge
)

target_link_libraries(ui_node
    ${OpenCV_LIBRARIES}
    Qt${QT_VERSION_MAJOR}::Widgets
)


install(TARGETS
    camera_node
    ui_node
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
    launch
    config
    DESTINATION share/${PROJECT_NAME}
)

ament_package()