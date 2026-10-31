cmake_minimum_required(VERSION 3.8)
project(central_logger)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(custom_interfaces REQUIRED)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

# Library other nodes link to publish logs to the central logger
add_library(log_publisher SHARED
  src/log_publisher.cpp
)
target_compile_features(log_publisher PUBLIC cxx_std_17)
target_include_directories(log_publisher PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(log_publisher
  rclcpp
  custom_interfaces
)

# Main executable (without ENABLE_TESTING defined)
add_executable(central_logger src/central_logger.cpp)
target_compile_features(central_logger PUBLIC cxx_std_17)
ament_target_dependencies(central_logger
  rclcpp
  custom_interfaces
  std_msgs
)

# Install main executable
install(TARGETS
  central_logger
  DESTINATION lib/${PROJECT_NAME})

# Install log_publisher library and header
install(
  TARGETS log_publisher
  EXPORT central_loggerTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  FILES include/log_publisher.hpp
  DESTINATION include
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  
  # Create a library from the source for testing (WITH ENABLE_TESTING defined)
  add_library(${PROJECT_NAME}_lib src/central_logger.cpp)
  target_compile_features(${PROJECT_NAME}_lib PUBLIC cxx_std_17)
  target_compile_definitions(${PROJECT_NAME}_lib PUBLIC ENABLE_TESTING)
  ament_target_dependencies(${PROJECT_NAME}_lib
    rclcpp
    custom_interfaces
    std_msgs
  )
  target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  
  # Add test executable and link with library
  ament_add_ros_isolated_gtest(${PROJECT_NAME}_test tests/test_central_logger.cpp)
  target_compile_definitions(${PROJECT_NAME}_test PUBLIC ENABLE_TESTING)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib)
  target_include_directories(${PROJECT_NAME}_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  ament_target_dependencies(${PROJECT_NAME}_test
    rclcpp
    custom_interfaces
    std_msgs
  )
endif()

# Install launch files.
# install(DIRECTORY
# launch
# DESTINATION share/${PROJECT_NAME}/
# )

ament_export_targets(central_loggerTargets HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp custom_interfaces)

ament_package()
