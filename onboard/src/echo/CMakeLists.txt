cmake_minimum_required(VERSION 3.8)
project(echo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(custom_interfaces REQUIRED)
find_package(fd_interface REQUIRED)
find_package(tui_interface REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_executable(echo src/echo.cpp)
ament_target_dependencies(echo
  rclcpp
  std_msgs
  custom_interfaces
  fd_interface
  tui_interface
)

install(TARGETS
  echo
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  # Nothing for now
endif()

ament_package()
