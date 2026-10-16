// Copyright 2022-2024 The MathWorks, Inc.
// Generated 13-Jul-2026 00:21:14
#include "slros2_initialize.h"
const std::string SLROSNodeName("matlab_control");
// matlab_control/ /Subsystem/Publish
SimulinkPublisher<custom_interfaces::msg::Result,SL_Bus_custom_interfaces_Result> Pub_matlab_control_454_7;
// matlab_control/Subsystem Reference1/Publish
SimulinkPublisher<custom_interfaces::msg::Debug,SL_Bus_custom_interfaces_Debug> Pub_matlab_control_499_3;
// matlab_control/pwm_heartbeat_publisher/Publish2
SimulinkPublisher<std_msgs::msg::Empty,SL_Bus_std_msgs_Empty> Pub_matlab_control_530_150;
// matlab_control/pwm_publisher/Publish
SimulinkPublisher<custom_interfaces::msg::Pwms,SL_Bus_custom_interfaces_Pwms> Pub_matlab_control_531_2;
// matlab_control/ /Subscribe
SimulinkSubscriber<custom_interfaces::msg::Goal,SL_Bus_custom_interfaces_Goal> Sub_matlab_control_454_284;
// matlab_control/DRR
SimulinkSubscriber<custom_interfaces::msg::DRR,SL_Bus_custom_interfaces_DRR> Sub_matlab_control_457;
// matlab_control/IMU
SimulinkSubscriber<custom_interfaces::msg::Imu,SL_Bus_custom_interfaces_Imu> Sub_matlab_control_492;
// matlab_control/VR
SimulinkSubscriber<custom_interfaces::msg::VR,SL_Bus_custom_interfaces_VR> Sub_matlab_control_507;
// matlab_control/dvl_reset_subscriber/Subscribe
SimulinkSubscriber<std_msgs::msg::Bool,SL_Bus_std_msgs_Bool> Sub_matlab_control_511;
// matlab_control/joystick_output/Subscribe
SimulinkSubscriber<custom_interfaces::msg::Gamepad,SL_Bus_custom_interfaces_Gamepad> Sub_matlab_control_523;
