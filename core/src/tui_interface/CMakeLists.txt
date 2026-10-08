cmake_minimum_required(VERSION 3.8)
project(tui_interface)

find_package(ament_cmake REQUIRED)

add_library(tui_interface SHARED
  src/tui_interface.cpp
)

target_include_directories(tui_interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(
  TARGETS tui_interface
  EXPORT tui_interfaceTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_targets(tui_interfaceTargets HAS_LIBRARY_TARGET)
ament_export_include_directories(include)

ament_package()