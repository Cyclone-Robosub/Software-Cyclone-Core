cmake_minimum_required(VERSION 3.8)
project(fd_interface)

find_package(ament_cmake REQUIRED)

add_library(fd_interface SHARED
  src/fd_interface.cpp
)

target_include_directories(fd_interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(
  TARGETS fd_interface
  EXPORT fd_interfaceTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_targets(fd_interfaceTargets HAS_LIBRARY_TARGET)
ament_export_include_directories(include)

ament_package()