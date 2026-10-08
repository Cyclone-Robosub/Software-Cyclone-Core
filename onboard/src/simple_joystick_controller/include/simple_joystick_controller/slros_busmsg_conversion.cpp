#ifndef _SLROS_BUSMSG_CONVERSION_H_
#define _SLROS_BUSMSG_CONVERSION_H_

#include "rclcpp/rclcpp.hpp"
#include <custom_interfaces/msg/gamepad.hpp>
#include <custom_interfaces/msg/pwms.hpp>
#include <std_msgs/msg/empty.hpp>
#include "Simple_Joystick_Controller_types.h"
#include "slros_msgconvert_utils.h"


[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Gamepad& msgPtr, SL_Bus_custom_interfaces_Gamepad const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Gamepad* busPtr, const custom_interfaces::msg::Gamepad& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Pwms& msgPtr, SL_Bus_custom_interfaces_Pwms const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Pwms* busPtr, const custom_interfaces::msg::Pwms& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Empty& msgPtr, SL_Bus_std_msgs_Empty const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Empty* busPtr, const std_msgs::msg::Empty& msgPtr);



// Conversions between SL_Bus_custom_interfaces_Gamepad and custom_interfaces::msg::Gamepad

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Gamepad& msgPtr, SL_Bus_custom_interfaces_Gamepad const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Gamepad");

  msgPtr.bumper_left =  busPtr->bumper_left;
  msgPtr.bumper_right =  busPtr->bumper_right;
  msgPtr.circle_button =  busPtr->circle_button;
  msgPtr.cross_button =  busPtr->cross_button;
  msgPtr.dpad_down =  busPtr->dpad_down;
  msgPtr.dpad_left =  busPtr->dpad_left;
  msgPtr.dpad_right =  busPtr->dpad_right;
  msgPtr.dpad_up =  busPtr->dpad_up;
  msgPtr.joystick_press_left =  busPtr->joystick_press_left;
  msgPtr.joystick_press_right =  busPtr->joystick_press_right;
  msgPtr.pitch =  busPtr->pitch;
  msgPtr.rise =  busPtr->rise;
  msgPtr.select =  busPtr->select;
  msgPtr.sink =  busPtr->sink;
  msgPtr.square_button =  busPtr->square_button;
  msgPtr.start =  busPtr->start;
  msgPtr.triangle_button =  busPtr->triangle_button;
  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.yaw =  busPtr->yaw;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Gamepad* busPtr, const custom_interfaces::msg::Gamepad& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Gamepad");

  busPtr->bumper_left =  msgPtr.bumper_left;
  busPtr->bumper_right =  msgPtr.bumper_right;
  busPtr->circle_button =  msgPtr.circle_button;
  busPtr->cross_button =  msgPtr.cross_button;
  busPtr->dpad_down =  msgPtr.dpad_down;
  busPtr->dpad_left =  msgPtr.dpad_left;
  busPtr->dpad_right =  msgPtr.dpad_right;
  busPtr->dpad_up =  msgPtr.dpad_up;
  busPtr->joystick_press_left =  msgPtr.joystick_press_left;
  busPtr->joystick_press_right =  msgPtr.joystick_press_right;
  busPtr->pitch =  msgPtr.pitch;
  busPtr->rise =  msgPtr.rise;
  busPtr->select =  msgPtr.select;
  busPtr->sink =  msgPtr.sink;
  busPtr->square_button =  msgPtr.square_button;
  busPtr->start =  msgPtr.start;
  busPtr->triangle_button =  msgPtr.triangle_button;
  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->yaw =  msgPtr.yaw;
}


// Conversions between SL_Bus_custom_interfaces_Pwms and custom_interfaces::msg::Pwms

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Pwms& msgPtr, SL_Bus_custom_interfaces_Pwms const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Pwms");

  convertFromBusFixedPrimitiveArray(msgPtr.pwms, busPtr->pwms);
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Pwms* busPtr, const custom_interfaces::msg::Pwms& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Pwms");

  convertToBusFixedPrimitiveArray(busPtr->pwms, msgPtr.pwms, slros::NoopWarning());
}


// Conversions between SL_Bus_std_msgs_Empty and std_msgs::msg::Empty

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Empty& msgPtr, SL_Bus_std_msgs_Empty const* busPtr)
{
  const std::string rosMessageType("std_msgs/Empty");

}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Empty* busPtr, const std_msgs::msg::Empty& msgPtr)
{
  const std::string rosMessageType("std_msgs/Empty");

}



#endif
