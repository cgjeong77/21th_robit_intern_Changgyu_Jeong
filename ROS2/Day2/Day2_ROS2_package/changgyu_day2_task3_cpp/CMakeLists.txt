cmake_minimum_required(VERSION 3.8)
project(changgyu_day2_task3_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

include_directories(include)

add_executable(talker_node
  src/talker.cpp
  ui/talker.ui
)

ament_target_dependencies(
  talker_node
  rclcpp
  std_msgs
)

target_link_libraries(
  talker_node
  Qt5::Widgets
)

add_executable(listener_node
  src/listener.cpp
  ui/listener.ui
)

ament_target_dependencies(
  listener_node
  rclcpp
  std_msgs
)

target_link_libraries(
  listener_node
  Qt5::Widgets
)

install(
  TARGETS
  talker_node
  listener_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()