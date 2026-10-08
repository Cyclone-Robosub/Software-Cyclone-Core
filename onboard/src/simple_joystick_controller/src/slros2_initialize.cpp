// Copyright 2022-2024 The MathWorks, Inc.
// Generated 14-May-2026 23:42:45
#include "slros2_initialize.h"
const std::string SLROSNodeName("Simple_Joystick_Controller");
// Simple_Joystick_Controller/pwm_heartbeat_publisher/Publish2
SimulinkPublisher<std_msgs::msg::Empty,SL_Bus_std_msgs_Empty> Pub_Simple_Joystick_Controller_85_150;
// Simple_Joystick_Controller/pwm_publisher/Publish
SimulinkPublisher<custom_interfaces::msg::Pwms,SL_Bus_custom_interfaces_Pwms> Pub_Simple_Joystick_Controller_86_2;
// Simple_Joystick_Controller/joystick_output/Subscribe
SimulinkSubscriber<custom_interfaces::msg::Gamepad,SL_Bus_custom_interfaces_Gamepad> Sub_Simple_Joystick_Controller_77;
