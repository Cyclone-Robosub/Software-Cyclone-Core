cmake_minimum_required(VERSION 3.8)
project(bms)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(rclcpp REQUIRED)
find_package(custom_interfaces REQUIRED)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

# Main executable (without ENABLE_TESTING defined)
add_executable(bms src/simple_bms.cpp)
ament_target_dependencies(bms
  rclcpp
  custom_interfaces
)

# Install main executable
install(TARGETS
  bms
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  
  # Create a library from the source for testing (WITH ENABLE_TESTING defined)
  add_library(${PROJECT_NAME}_lib src/simple_bms.cpp)
  target_compile_definitions(${PROJECT_NAME}_lib PUBLIC ENABLE_TESTING)
  ament_target_dependencies(${PROJECT_NAME}_lib
    rclcpp
    custom_interfaces
  )
  target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  
  # Add test executable and link with library
  ament_add_ros_isolated_gtest(${PROJECT_NAME}_test test/test_simple_bms.cpp)
  target_compile_definitions(${PROJECT_NAME}_test PUBLIC ENABLE_TESTING)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib)
  target_include_directories(${PROJECT_NAME}_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  ament_target_dependencies(${PROJECT_NAME}_test
    rclcpp
    custom_interfaces
  )
endif()

# Install launch files.
install(DIRECTORY
launch
DESTINATION share/${PROJECT_NAME}/
)

ament_package()