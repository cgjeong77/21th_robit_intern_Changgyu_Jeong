cmake_minimum_required(VERSION 3.8)
project(knife_hit_game)

# 컴파일 경고 표시
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# ==================================================
# Qt 자동 처리 설정
# ==================================================

# .ui 파일 자동 변환
set(CMAKE_AUTOUIC ON)

# Q_OBJECT, Signal/Slot 자동 처리
set(CMAKE_AUTOMOC ON)

# .qrc 리소스 자동 처리
set(CMAKE_AUTORCC ON)

# Qt Designer UI 파일 위치
set(CMAKE_AUTOUIC_SEARCH_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
)


# ==================================================
# 필요한 패키지
# ==================================================

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets)


# ==================================================
# Game Logic Node
# ==================================================

add_executable(
  game_logic_node
  src/game_logic_node.cpp
)

ament_target_dependencies(
  game_logic_node
  rclcpp
  std_msgs
  std_srvs
)


# ==================================================
# Qt GUI Node
# ==================================================

add_executable(
  game_gui_node

  # 실행 및 화면 코드
  src/main.cpp
  src/main_window.cpp
  src/qnode.cpp

  # Q_OBJECT가 들어간 헤더
  include/knife_hit_game/main_window.hpp
  include/knife_hit_game/qnode.hpp

  # Designer UI
  ui/mainwindow.ui

  # 이미지 리소스
  resources/images.qrc
)


# 헤더 파일 위치
target_include_directories(
  game_gui_node
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# ROS2 의존성
ament_target_dependencies(
  game_gui_node
  rclcpp
  std_msgs
  std_srvs
)


# Qt5 Widgets 연결
target_link_libraries(
  game_gui_node
  Qt5::Widgets
)


# ==================================================
# 실행 파일 설치
# ==================================================

install(
  TARGETS
    game_logic_node
    game_gui_node

  DESTINATION
    lib/${PROJECT_NAME}
)


ament_package()