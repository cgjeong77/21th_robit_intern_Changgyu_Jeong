cmake_minimum_required(VERSION 3.8)
project(changgyu_day2_task1_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(changgyu_day2_interfaces REQUIRED)

include_directories(
  include
  ${changgyu_day2_interfaces_INCLUDE_DIRS}
)

add_executable(publisher_node
  src/publisher.cpp
)

ament_target_dependencies(
  publisher_node
  rclcpp
  changgyu_day2_interfaces
)

add_executable(subscriber_node
  src/subscriber.cpp
)

ament_target_dependencies(
  subscriber_node
  rclcpp
  changgyu_day2_interfaces
)

install(
  TARGETS
  publisher_node
  subscriber_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()
endif()

ament_package()