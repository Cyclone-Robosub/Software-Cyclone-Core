cmake_minimum_required(VERSION 3.8)
project(mux_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(custom_interfaces REQUIRED)
find_package(tui_interface REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

# Main executable (without ENABLE_TESTING defined)
add_executable(mux_controller src/mux_controller.cpp)
ament_target_dependencies(mux_controller
  rclcpp
  std_msgs
  std_srvs
  custom_interfaces
  tui_interface
)

# Install main executable
install(TARGETS
  mux_controller
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  # Nothing yet
endif()

ament_package()
