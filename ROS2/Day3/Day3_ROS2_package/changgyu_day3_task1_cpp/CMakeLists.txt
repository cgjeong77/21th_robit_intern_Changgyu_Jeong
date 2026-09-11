cmake_minimum_required(VERSION 3.8)
project(changgyu_day3_task1_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(std_srvs REQUIRED)

include_directories(include)

add_executable(task1_node
  src/task1.cpp
)

ament_target_dependencies(
  task1_node
  rclcpp
  geometry_msgs
  turtlesim
  std_srvs
)

install(
  TARGETS task1_node
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()