
//
// File main.cpp
//
// Code generated for Simulink model 'Simple_Joystick_Controller'.
//
// Model version                  : 1.16
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu May 14 23:42:47 2026
//
#include "ros2nodeinterface.h"
rclcpp::Node::SharedPtr SLROSNodePtr;
namespace ros2 {
namespace matlab {
  std::shared_ptr<ros2::matlab::NodeInterface> gMatlabNodeIntr;
  std::shared_ptr<ros2::matlab::NodeInterface> getNodeInterface() {
    return gMatlabNodeIntr;
  }
} //namespace matlab
} //namespace ros2
int main(int argc, char* argv[]) {
#ifdef MW_DEBUG_LOG
  #ifdef ROS2_DISTRO_JAZZY //jazzy
   std::cout<<"\nThis application is built for ROS 2 Jazzy\n\n";
  #elif defined(ROS2_DISTRO_HUMBLE) //humble
   std::cout<<"\nThis application is built for ROS 2 Humble\n\n";
  #endif
#endif
    ros2::matlab::gMatlabNodeIntr = std::make_shared<ros2::matlab::NodeInterface>();
    ros2::matlab::gMatlabNodeIntr->initialize(argc, argv);
    auto ret = ros2::matlab::gMatlabNodeIntr->run();
    ros2::matlab::gMatlabNodeIntr->terminate();
    ros2::matlab::gMatlabNodeIntr.reset();
    return ret;
}
