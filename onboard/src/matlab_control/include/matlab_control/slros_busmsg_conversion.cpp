#ifndef _SLROS_BUSMSG_CONVERSION_H_
#define _SLROS_BUSMSG_CONVERSION_H_

#include "rclcpp/rclcpp.hpp"
#include <builtin_interfaces/msg/time.hpp>
#include <custom_interfaces/msg/drr.hpp>
#include <custom_interfaces/msg/debug.hpp>
#include <custom_interfaces/msg/gamepad.hpp>
#include <custom_interfaces/msg/goal.hpp>
#include <custom_interfaces/msg/imu.hpp>
#include <custom_interfaces/msg/pose6_d.hpp>
#include <custom_interfaces/msg/pwms.hpp>
#include <custom_interfaces/msg/result.hpp>
#include <custom_interfaces/msg/vr.hpp>
#include <custom_interfaces/msg/waypoint_mask.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/twist_with_covariance.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/fluid_pressure.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/float32_multi_array.hpp>
#include <std_msgs/msg/header.hpp>
#include <std_msgs/msg/multi_array_dimension.hpp>
#include <std_msgs/msg/multi_array_layout.hpp>
#include "matlab_control_types.h"
#include "slros_msgconvert_utils.h"


[[maybe_unused]] static void convertFromBus(builtin_interfaces::msg::Time& msgPtr, SL_Bus_builtin_interfaces_Time const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_builtin_interfaces_Time* busPtr, const builtin_interfaces::msg::Time& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::DRR& msgPtr, SL_Bus_custom_interfaces_DRR const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_DRR* busPtr, const custom_interfaces::msg::DRR& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Debug& msgPtr, SL_Bus_custom_interfaces_Debug const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Debug* busPtr, const custom_interfaces::msg::Debug& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Gamepad& msgPtr, SL_Bus_custom_interfaces_Gamepad const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Gamepad* busPtr, const custom_interfaces::msg::Gamepad& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Goal& msgPtr, SL_Bus_custom_interfaces_Goal const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Goal* busPtr, const custom_interfaces::msg::Goal& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Imu& msgPtr, SL_Bus_custom_interfaces_Imu const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Imu* busPtr, const custom_interfaces::msg::Imu& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Pose6D& msgPtr, SL_Bus_custom_interfaces_Pose6D const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Pose6D* busPtr, const custom_interfaces::msg::Pose6D& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Pwms& msgPtr, SL_Bus_custom_interfaces_Pwms const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Pwms* busPtr, const custom_interfaces::msg::Pwms& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Result& msgPtr, SL_Bus_custom_interfaces_Result const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Result* busPtr, const custom_interfaces::msg::Result& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::VR& msgPtr, SL_Bus_custom_interfaces_VR const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_VR* busPtr, const custom_interfaces::msg::VR& msgPtr);

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::WaypointMask& msgPtr, SL_Bus_custom_interfaces_WaypointMask const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_WaypointMask* busPtr, const custom_interfaces::msg::WaypointMask& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Point& msgPtr, SL_Bus_geometry_msgs_Point const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Point* busPtr, const geometry_msgs::msg::Point& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Pose& msgPtr, SL_Bus_geometry_msgs_Pose const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Pose* busPtr, const geometry_msgs::msg::Pose& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::PoseWithCovariance& msgPtr, SL_Bus_geometry_msgs_PoseWithCovariance const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_PoseWithCovariance* busPtr, const geometry_msgs::msg::PoseWithCovariance& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Quaternion& msgPtr, SL_Bus_geometry_msgs_Quaternion const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Quaternion* busPtr, const geometry_msgs::msg::Quaternion& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Twist& msgPtr, SL_Bus_geometry_msgs_Twist const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Twist* busPtr, const geometry_msgs::msg::Twist& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::TwistWithCovariance& msgPtr, SL_Bus_geometry_msgs_TwistWithCovariance const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_TwistWithCovariance* busPtr, const geometry_msgs::msg::TwistWithCovariance& msgPtr);

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Vector3& msgPtr, SL_Bus_geometry_msgs_Vector3 const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Vector3* busPtr, const geometry_msgs::msg::Vector3& msgPtr);

[[maybe_unused]] static void convertFromBus(nav_msgs::msg::Odometry& msgPtr, SL_Bus_nav_msgs_Odometry const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_nav_msgs_Odometry* busPtr, const nav_msgs::msg::Odometry& msgPtr);

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::FluidPressure& msgPtr, SL_Bus_sensor_msgs_FluidPressure const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_FluidPressure* busPtr, const sensor_msgs::msg::FluidPressure& msgPtr);

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::Imu& msgPtr, SL_Bus_sensor_msgs_Imu const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_Imu* busPtr, const sensor_msgs::msg::Imu& msgPtr);

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::MagneticField& msgPtr, SL_Bus_sensor_msgs_MagneticField const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_MagneticField* busPtr, const sensor_msgs::msg::MagneticField& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Bool& msgPtr, SL_Bus_std_msgs_Bool const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Bool* busPtr, const std_msgs::msg::Bool& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Empty& msgPtr, SL_Bus_std_msgs_Empty const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Empty* busPtr, const std_msgs::msg::Empty& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Float32MultiArray& msgPtr, SL_Bus_std_msgs_Float32MultiArray const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Float32MultiArray* busPtr, const std_msgs::msg::Float32MultiArray& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Header& msgPtr, SL_Bus_std_msgs_Header const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Header* busPtr, const std_msgs::msg::Header& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::MultiArrayDimension& msgPtr, SL_Bus_std_msgs_MultiArrayDimension const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_MultiArrayDimension* busPtr, const std_msgs::msg::MultiArrayDimension& msgPtr);

[[maybe_unused]] static void convertFromBus(std_msgs::msg::MultiArrayLayout& msgPtr, SL_Bus_std_msgs_MultiArrayLayout const* busPtr);
[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_MultiArrayLayout* busPtr, const std_msgs::msg::MultiArrayLayout& msgPtr);



// Conversions between SL_Bus_builtin_interfaces_Time and builtin_interfaces::msg::Time

[[maybe_unused]] static void convertFromBus(builtin_interfaces::msg::Time& msgPtr, SL_Bus_builtin_interfaces_Time const* busPtr)
{
  const std::string rosMessageType("builtin_interfaces/Time");

  msgPtr.nanosec =  busPtr->nanosec;
  msgPtr.sec =  busPtr->sec;
}

[[maybe_unused]] static void convertToBus(SL_Bus_builtin_interfaces_Time* busPtr, const builtin_interfaces::msg::Time& msgPtr)
{
  const std::string rosMessageType("builtin_interfaces/Time");

  busPtr->nanosec =  msgPtr.nanosec;
  busPtr->sec =  msgPtr.sec;
}


// Conversions between SL_Bus_custom_interfaces_DRR and custom_interfaces::msg::DRR

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::DRR& msgPtr, SL_Bus_custom_interfaces_DRR const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/DRR");

  convertFromBus(msgPtr.angle, &busPtr->angle);
  msgPtr.pos_std =  busPtr->pos_std;
  convertFromBus(msgPtr.position, &busPtr->position);
  msgPtr.status =  busPtr->status;
  msgPtr.time_stamp = (int64_t) busPtr->time_stamp;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_DRR* busPtr, const custom_interfaces::msg::DRR& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/DRR");

  convertToBus(&busPtr->angle, msgPtr.angle);
  busPtr->pos_std =  msgPtr.pos_std;
  convertToBus(&busPtr->position, msgPtr.position);
  busPtr->status =  msgPtr.status;
  busPtr->time_stamp = (real_T) msgPtr.time_stamp;
}


// Conversions between SL_Bus_custom_interfaces_Debug and custom_interfaces::msg::Debug

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Debug& msgPtr, SL_Bus_custom_interfaces_Debug const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Debug");

  convertFromBusFixedPrimitiveArray(msgPtr.message, busPtr->message);
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Debug* busPtr, const custom_interfaces::msg::Debug& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Debug");

  convertToBusFixedPrimitiveArray(busPtr->message, msgPtr.message, slros::NoopWarning());
}


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


// Conversions between SL_Bus_custom_interfaces_Goal and custom_interfaces::msg::Goal

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Goal& msgPtr, SL_Bus_custom_interfaces_Goal const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Goal");

  convertFromBusFixedPrimitiveArray(msgPtr.command_id, busPtr->command_id);
  msgPtr.confidence =  busPtr->confidence;
  msgPtr.duration =  busPtr->duration;
  msgPtr.hold_time =  busPtr->hold_time;
  convertFromBusFixedPrimitiveArray(msgPtr.object, busPtr->object);
  convertFromBus(msgPtr.tolerance, &busPtr->tolerance);
  convertFromBusFixedPrimitiveArray(msgPtr.trick, busPtr->trick);
  convertFromBus(msgPtr.waypoint, &busPtr->waypoint);
  convertFromBus(msgPtr.waypoint_mask, &busPtr->waypoint_mask);
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Goal* busPtr, const custom_interfaces::msg::Goal& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Goal");

  convertToBusFixedPrimitiveArray(busPtr->command_id, msgPtr.command_id, slros::NoopWarning());
  busPtr->confidence =  msgPtr.confidence;
  busPtr->duration =  msgPtr.duration;
  busPtr->hold_time =  msgPtr.hold_time;
  convertToBusFixedPrimitiveArray(busPtr->object, msgPtr.object, slros::NoopWarning());
  convertToBus(&busPtr->tolerance, msgPtr.tolerance);
  convertToBusFixedPrimitiveArray(busPtr->trick, msgPtr.trick, slros::NoopWarning());
  convertToBus(&busPtr->waypoint, msgPtr.waypoint);
  convertToBus(&busPtr->waypoint_mask, msgPtr.waypoint_mask);
}


// Conversions between SL_Bus_custom_interfaces_Imu and custom_interfaces::msg::Imu

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Imu& msgPtr, SL_Bus_custom_interfaces_Imu const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Imu");

  convertFromBus(msgPtr.ahrs_database, &busPtr->ahrs_database);
  convertFromBus(msgPtr.imu_fusion, &busPtr->imu_fusion);
  convertFromBus(msgPtr.mag_array, &busPtr->mag_array);
  msgPtr.pitch =  busPtr->pitch;
  convertFromBus(msgPtr.pressure, &busPtr->pressure);
  msgPtr.roll =  busPtr->roll;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Imu* busPtr, const custom_interfaces::msg::Imu& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Imu");

  convertToBus(&busPtr->ahrs_database, msgPtr.ahrs_database);
  convertToBus(&busPtr->imu_fusion, msgPtr.imu_fusion);
  convertToBus(&busPtr->mag_array, msgPtr.mag_array);
  busPtr->pitch =  msgPtr.pitch;
  convertToBus(&busPtr->pressure, msgPtr.pressure);
  busPtr->roll =  msgPtr.roll;
}


// Conversions between SL_Bus_custom_interfaces_Pose6D and custom_interfaces::msg::Pose6D

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Pose6D& msgPtr, SL_Bus_custom_interfaces_Pose6D const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Pose6D");

  msgPtr.pitch =  busPtr->pitch;
  msgPtr.roll =  busPtr->roll;
  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.yaw =  busPtr->yaw;
  msgPtr.z =  busPtr->z;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Pose6D* busPtr, const custom_interfaces::msg::Pose6D& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Pose6D");

  busPtr->pitch =  msgPtr.pitch;
  busPtr->roll =  msgPtr.roll;
  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->yaw =  msgPtr.yaw;
  busPtr->z =  msgPtr.z;
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


// Conversions between SL_Bus_custom_interfaces_Result and custom_interfaces::msg::Result

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::Result& msgPtr, SL_Bus_custom_interfaces_Result const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/Result");

  convertFromBusFixedPrimitiveArray(msgPtr.found_object, busPtr->found_object);
  msgPtr.reached_waypoint_without_detection =  busPtr->reached_waypoint_without_detection;
  msgPtr.success =  busPtr->success;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_Result* busPtr, const custom_interfaces::msg::Result& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/Result");

  convertToBusFixedPrimitiveArray(busPtr->found_object, msgPtr.found_object, slros::NoopWarning());
  busPtr->reached_waypoint_without_detection =  msgPtr.reached_waypoint_without_detection;
  busPtr->success =  msgPtr.success;
}


// Conversions between SL_Bus_custom_interfaces_VR and custom_interfaces::msg::VR

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::VR& msgPtr, SL_Bus_custom_interfaces_VR const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/VR");

  msgPtr.altitude =  busPtr->altitude;
  convertFromBus(msgPtr.covariance, &busPtr->covariance);
  msgPtr.fom =  busPtr->fom;
  msgPtr.status =  busPtr->status;
  msgPtr.time =  busPtr->time;
  msgPtr.time_of_transmission = (int64_t) busPtr->time_of_transmission;
  msgPtr.time_of_validity = (int64_t) busPtr->time_of_validity;
  msgPtr.valid =  busPtr->valid;
  convertFromBus(msgPtr.velocity_data, &busPtr->velocity_data);
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_VR* busPtr, const custom_interfaces::msg::VR& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/VR");

  busPtr->altitude =  msgPtr.altitude;
  convertToBus(&busPtr->covariance, msgPtr.covariance);
  busPtr->fom =  msgPtr.fom;
  busPtr->status =  msgPtr.status;
  busPtr->time =  msgPtr.time;
  busPtr->time_of_transmission = (real_T) msgPtr.time_of_transmission;
  busPtr->time_of_validity = (real_T) msgPtr.time_of_validity;
  busPtr->valid =  msgPtr.valid;
  convertToBus(&busPtr->velocity_data, msgPtr.velocity_data);
}


// Conversions between SL_Bus_custom_interfaces_WaypointMask and custom_interfaces::msg::WaypointMask

[[maybe_unused]] static void convertFromBus(custom_interfaces::msg::WaypointMask& msgPtr, SL_Bus_custom_interfaces_WaypointMask const* busPtr)
{
  const std::string rosMessageType("custom_interfaces/WaypointMask");

  msgPtr.pitch =  busPtr->pitch;
  msgPtr.roll =  busPtr->roll;
  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.yaw =  busPtr->yaw;
  msgPtr.z =  busPtr->z;
}

[[maybe_unused]] static void convertToBus(SL_Bus_custom_interfaces_WaypointMask* busPtr, const custom_interfaces::msg::WaypointMask& msgPtr)
{
  const std::string rosMessageType("custom_interfaces/WaypointMask");

  busPtr->pitch =  msgPtr.pitch;
  busPtr->roll =  msgPtr.roll;
  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->yaw =  msgPtr.yaw;
  busPtr->z =  msgPtr.z;
}


// Conversions between SL_Bus_geometry_msgs_Point and geometry_msgs::msg::Point

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Point& msgPtr, SL_Bus_geometry_msgs_Point const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/Point");

  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.z =  busPtr->z;
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Point* busPtr, const geometry_msgs::msg::Point& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/Point");

  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->z =  msgPtr.z;
}


// Conversions between SL_Bus_geometry_msgs_Pose and geometry_msgs::msg::Pose

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Pose& msgPtr, SL_Bus_geometry_msgs_Pose const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/Pose");

  convertFromBus(msgPtr.orientation, &busPtr->orientation);
  convertFromBus(msgPtr.position, &busPtr->position);
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Pose* busPtr, const geometry_msgs::msg::Pose& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/Pose");

  convertToBus(&busPtr->orientation, msgPtr.orientation);
  convertToBus(&busPtr->position, msgPtr.position);
}


// Conversions between SL_Bus_geometry_msgs_PoseWithCovariance and geometry_msgs::msg::PoseWithCovariance

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::PoseWithCovariance& msgPtr, SL_Bus_geometry_msgs_PoseWithCovariance const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/PoseWithCovariance");

  convertFromBusFixedPrimitiveArray(msgPtr.covariance, busPtr->covariance);
  convertFromBus(msgPtr.pose, &busPtr->pose);
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_PoseWithCovariance* busPtr, const geometry_msgs::msg::PoseWithCovariance& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/PoseWithCovariance");

  convertToBusFixedPrimitiveArray(busPtr->covariance, msgPtr.covariance, slros::NoopWarning());
  convertToBus(&busPtr->pose, msgPtr.pose);
}


// Conversions between SL_Bus_geometry_msgs_Quaternion and geometry_msgs::msg::Quaternion

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Quaternion& msgPtr, SL_Bus_geometry_msgs_Quaternion const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/Quaternion");

  msgPtr.w =  busPtr->w;
  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.z =  busPtr->z;
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Quaternion* busPtr, const geometry_msgs::msg::Quaternion& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/Quaternion");

  busPtr->w =  msgPtr.w;
  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->z =  msgPtr.z;
}


// Conversions between SL_Bus_geometry_msgs_Twist and geometry_msgs::msg::Twist

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Twist& msgPtr, SL_Bus_geometry_msgs_Twist const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/Twist");

  convertFromBus(msgPtr.angular, &busPtr->angular);
  convertFromBus(msgPtr.linear, &busPtr->linear);
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Twist* busPtr, const geometry_msgs::msg::Twist& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/Twist");

  convertToBus(&busPtr->angular, msgPtr.angular);
  convertToBus(&busPtr->linear, msgPtr.linear);
}


// Conversions between SL_Bus_geometry_msgs_TwistWithCovariance and geometry_msgs::msg::TwistWithCovariance

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::TwistWithCovariance& msgPtr, SL_Bus_geometry_msgs_TwistWithCovariance const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/TwistWithCovariance");

  convertFromBusFixedPrimitiveArray(msgPtr.covariance, busPtr->covariance);
  convertFromBus(msgPtr.twist, &busPtr->twist);
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_TwistWithCovariance* busPtr, const geometry_msgs::msg::TwistWithCovariance& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/TwistWithCovariance");

  convertToBusFixedPrimitiveArray(busPtr->covariance, msgPtr.covariance, slros::NoopWarning());
  convertToBus(&busPtr->twist, msgPtr.twist);
}


// Conversions between SL_Bus_geometry_msgs_Vector3 and geometry_msgs::msg::Vector3

[[maybe_unused]] static void convertFromBus(geometry_msgs::msg::Vector3& msgPtr, SL_Bus_geometry_msgs_Vector3 const* busPtr)
{
  const std::string rosMessageType("geometry_msgs/Vector3");

  msgPtr.x =  busPtr->x;
  msgPtr.y =  busPtr->y;
  msgPtr.z =  busPtr->z;
}

[[maybe_unused]] static void convertToBus(SL_Bus_geometry_msgs_Vector3* busPtr, const geometry_msgs::msg::Vector3& msgPtr)
{
  const std::string rosMessageType("geometry_msgs/Vector3");

  busPtr->x =  msgPtr.x;
  busPtr->y =  msgPtr.y;
  busPtr->z =  msgPtr.z;
}


// Conversions between SL_Bus_nav_msgs_Odometry and nav_msgs::msg::Odometry

[[maybe_unused]] static void convertFromBus(nav_msgs::msg::Odometry& msgPtr, SL_Bus_nav_msgs_Odometry const* busPtr)
{
  const std::string rosMessageType("nav_msgs/Odometry");

  convertFromBusVariablePrimitiveArray(msgPtr.child_frame_id, busPtr->child_frame_id, busPtr->child_frame_id_SL_Info);
  convertFromBus(msgPtr.header, &busPtr->header);
  convertFromBus(msgPtr.pose, &busPtr->pose);
  convertFromBus(msgPtr.twist, &busPtr->twist);
}

[[maybe_unused]] static void convertToBus(SL_Bus_nav_msgs_Odometry* busPtr, const nav_msgs::msg::Odometry& msgPtr)
{
  const std::string rosMessageType("nav_msgs/Odometry");

  convertToBusVariablePrimitiveArray(busPtr->child_frame_id, busPtr->child_frame_id_SL_Info, msgPtr.child_frame_id, slros::EnabledWarning(rosMessageType, "child_frame_id"));
  convertToBus(&busPtr->header, msgPtr.header);
  convertToBus(&busPtr->pose, msgPtr.pose);
  convertToBus(&busPtr->twist, msgPtr.twist);
}


// Conversions between SL_Bus_sensor_msgs_FluidPressure and sensor_msgs::msg::FluidPressure

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::FluidPressure& msgPtr, SL_Bus_sensor_msgs_FluidPressure const* busPtr)
{
  const std::string rosMessageType("sensor_msgs/FluidPressure");

  msgPtr.fluid_pressure =  busPtr->fluid_pressure;
  convertFromBus(msgPtr.header, &busPtr->header);
  msgPtr.variance =  busPtr->variance;
}

[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_FluidPressure* busPtr, const sensor_msgs::msg::FluidPressure& msgPtr)
{
  const std::string rosMessageType("sensor_msgs/FluidPressure");

  busPtr->fluid_pressure =  msgPtr.fluid_pressure;
  convertToBus(&busPtr->header, msgPtr.header);
  busPtr->variance =  msgPtr.variance;
}


// Conversions between SL_Bus_sensor_msgs_Imu and sensor_msgs::msg::Imu

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::Imu& msgPtr, SL_Bus_sensor_msgs_Imu const* busPtr)
{
  const std::string rosMessageType("sensor_msgs/Imu");

  convertFromBus(msgPtr.angular_velocity, &busPtr->angular_velocity);
  convertFromBusFixedPrimitiveArray(msgPtr.angular_velocity_covariance, busPtr->angular_velocity_covariance);
  convertFromBus(msgPtr.header, &busPtr->header);
  convertFromBus(msgPtr.linear_acceleration, &busPtr->linear_acceleration);
  convertFromBusFixedPrimitiveArray(msgPtr.linear_acceleration_covariance, busPtr->linear_acceleration_covariance);
  convertFromBus(msgPtr.orientation, &busPtr->orientation);
  convertFromBusFixedPrimitiveArray(msgPtr.orientation_covariance, busPtr->orientation_covariance);
}

[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_Imu* busPtr, const sensor_msgs::msg::Imu& msgPtr)
{
  const std::string rosMessageType("sensor_msgs/Imu");

  convertToBus(&busPtr->angular_velocity, msgPtr.angular_velocity);
  convertToBusFixedPrimitiveArray(busPtr->angular_velocity_covariance, msgPtr.angular_velocity_covariance, slros::NoopWarning());
  convertToBus(&busPtr->header, msgPtr.header);
  convertToBus(&busPtr->linear_acceleration, msgPtr.linear_acceleration);
  convertToBusFixedPrimitiveArray(busPtr->linear_acceleration_covariance, msgPtr.linear_acceleration_covariance, slros::NoopWarning());
  convertToBus(&busPtr->orientation, msgPtr.orientation);
  convertToBusFixedPrimitiveArray(busPtr->orientation_covariance, msgPtr.orientation_covariance, slros::NoopWarning());
}


// Conversions between SL_Bus_sensor_msgs_MagneticField and sensor_msgs::msg::MagneticField

[[maybe_unused]] static void convertFromBus(sensor_msgs::msg::MagneticField& msgPtr, SL_Bus_sensor_msgs_MagneticField const* busPtr)
{
  const std::string rosMessageType("sensor_msgs/MagneticField");

  convertFromBus(msgPtr.header, &busPtr->header);
  convertFromBus(msgPtr.magnetic_field, &busPtr->magnetic_field);
  convertFromBusFixedPrimitiveArray(msgPtr.magnetic_field_covariance, busPtr->magnetic_field_covariance);
}

[[maybe_unused]] static void convertToBus(SL_Bus_sensor_msgs_MagneticField* busPtr, const sensor_msgs::msg::MagneticField& msgPtr)
{
  const std::string rosMessageType("sensor_msgs/MagneticField");

  convertToBus(&busPtr->header, msgPtr.header);
  convertToBus(&busPtr->magnetic_field, msgPtr.magnetic_field);
  convertToBusFixedPrimitiveArray(busPtr->magnetic_field_covariance, msgPtr.magnetic_field_covariance, slros::NoopWarning());
}


// Conversions between SL_Bus_std_msgs_Bool and std_msgs::msg::Bool

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Bool& msgPtr, SL_Bus_std_msgs_Bool const* busPtr)
{
  const std::string rosMessageType("std_msgs/Bool");

  msgPtr.data =  busPtr->data;
}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Bool* busPtr, const std_msgs::msg::Bool& msgPtr)
{
  const std::string rosMessageType("std_msgs/Bool");

  busPtr->data =  msgPtr.data;
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


// Conversions between SL_Bus_std_msgs_Float32MultiArray and std_msgs::msg::Float32MultiArray

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Float32MultiArray& msgPtr, SL_Bus_std_msgs_Float32MultiArray const* busPtr)
{
  const std::string rosMessageType("std_msgs/Float32MultiArray");

  convertFromBusVariablePrimitiveArray(msgPtr.data, busPtr->data, busPtr->data_SL_Info);
  convertFromBus(msgPtr.layout, &busPtr->layout);
}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Float32MultiArray* busPtr, const std_msgs::msg::Float32MultiArray& msgPtr)
{
  const std::string rosMessageType("std_msgs/Float32MultiArray");

  convertToBusVariablePrimitiveArray(busPtr->data, busPtr->data_SL_Info, msgPtr.data, slros::EnabledWarning(rosMessageType, "data"));
  convertToBus(&busPtr->layout, msgPtr.layout);
}


// Conversions between SL_Bus_std_msgs_Header and std_msgs::msg::Header

[[maybe_unused]] static void convertFromBus(std_msgs::msg::Header& msgPtr, SL_Bus_std_msgs_Header const* busPtr)
{
  const std::string rosMessageType("std_msgs/Header");

  convertFromBusVariablePrimitiveArray(msgPtr.frame_id, busPtr->frame_id, busPtr->frame_id_SL_Info);
  convertFromBus(msgPtr.stamp, &busPtr->stamp);
}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_Header* busPtr, const std_msgs::msg::Header& msgPtr)
{
  const std::string rosMessageType("std_msgs/Header");

  convertToBusVariablePrimitiveArray(busPtr->frame_id, busPtr->frame_id_SL_Info, msgPtr.frame_id, slros::EnabledWarning(rosMessageType, "frame_id"));
  convertToBus(&busPtr->stamp, msgPtr.stamp);
}


// Conversions between SL_Bus_std_msgs_MultiArrayDimension and std_msgs::msg::MultiArrayDimension

[[maybe_unused]] static void convertFromBus(std_msgs::msg::MultiArrayDimension& msgPtr, SL_Bus_std_msgs_MultiArrayDimension const* busPtr)
{
  const std::string rosMessageType("std_msgs/MultiArrayDimension");

  convertFromBusVariablePrimitiveArray(msgPtr.label, busPtr->label, busPtr->label_SL_Info);
  msgPtr.size =  busPtr->size;
  msgPtr.stride =  busPtr->stride;
}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_MultiArrayDimension* busPtr, const std_msgs::msg::MultiArrayDimension& msgPtr)
{
  const std::string rosMessageType("std_msgs/MultiArrayDimension");

  convertToBusVariablePrimitiveArray(busPtr->label, busPtr->label_SL_Info, msgPtr.label, slros::EnabledWarning(rosMessageType, "label"));
  busPtr->size =  msgPtr.size;
  busPtr->stride =  msgPtr.stride;
}


// Conversions between SL_Bus_std_msgs_MultiArrayLayout and std_msgs::msg::MultiArrayLayout

[[maybe_unused]] static void convertFromBus(std_msgs::msg::MultiArrayLayout& msgPtr, SL_Bus_std_msgs_MultiArrayLayout const* busPtr)
{
  const std::string rosMessageType("std_msgs/MultiArrayLayout");

  msgPtr.data_offset =  busPtr->data_offset;
  convertFromBusVariableNestedArray(msgPtr.dim, busPtr->dim, busPtr->dim_SL_Info);
}

[[maybe_unused]] static void convertToBus(SL_Bus_std_msgs_MultiArrayLayout* busPtr, const std_msgs::msg::MultiArrayLayout& msgPtr)
{
  const std::string rosMessageType("std_msgs/MultiArrayLayout");

  busPtr->data_offset =  msgPtr.data_offset;
  convertToBusVariableNestedArray(busPtr->dim, busPtr->dim_SL_Info, msgPtr.dim, slros::EnabledWarning(rosMessageType, "dim"));
}



#endif
