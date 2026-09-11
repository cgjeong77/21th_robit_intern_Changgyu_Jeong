cmake_minimum_required(VERSION 3.8)
project(changgyu_day3_task2_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories(include)

# Traffic Light Node
add_executable(
  traffic_light_node
  src/traffic_light_node.cpp
)

ament_target_dependencies(
  traffic_light_node
  rclcpp
  std_msgs
)

# Vehicle Node
add_executable(
  vehicle_node
  src/vehicle_node.cpp
)

ament_target_dependencies(
  vehicle_node
  rclcpp
  std_msgs
)

# Qt GUI
add_executable(
  traffic_gui
  src/main.cpp
  src/mainwindow.cpp
  ui/mainwindow.ui
)

ament_target_dependencies(
  traffic_gui
  rclcpp
  std_msgs
)

target_link_libraries(
  traffic_gui
  Qt5::Widgets
)

install(
  TARGETS
    traffic_light_node
    vehicle_node
    traffic_gui
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY
    launch
    config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()
endif()

ament_package()