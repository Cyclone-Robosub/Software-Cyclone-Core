//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: matlab_control_types.h
//
// Code generated for Simulink model 'matlab_control'.
//
// Model version                  : 1.13
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Jul 13 00:21:06 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-A (64-bit)
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef matlab_control_types_h_
#define matlab_control_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Pose6D_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Pose6D_

// MsgType=custom_interfaces/Pose6D
struct SL_Bus_custom_interfaces_Pose6D
{
  real_T x;
  real_T y;
  real_T z;
  real_T roll;
  real_T pitch;
  real_T yaw;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_WaypointMask_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_WaypointMask_

// MsgType=custom_interfaces/WaypointMask
struct SL_Bus_custom_interfaces_WaypointMask
{
  boolean_T x;
  boolean_T y;
  boolean_T z;
  boolean_T roll;
  boolean_T pitch;
  boolean_T yaw;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Goal_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Goal_

// MsgType=custom_interfaces/Goal
struct SL_Bus_custom_interfaces_Goal
{
  // PrimitiveROSType=char
  uint8_T command_id[16];

  // MsgType=custom_interfaces/Pose6D
  SL_Bus_custom_interfaces_Pose6D waypoint;

  // MsgType=custom_interfaces/WaypointMask
  SL_Bus_custom_interfaces_WaypointMask waypoint_mask;

  // MsgType=custom_interfaces/Pose6D
  SL_Bus_custom_interfaces_Pose6D tolerance;
  real_T hold_time;

  // PrimitiveROSType=char
  uint8_T object[16];
  real_T confidence;

  // PrimitiveROSType=char
  uint8_T trick[16];
  real_T duration;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Result_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Result_

// MsgType=custom_interfaces/Result
struct SL_Bus_custom_interfaces_Result
{
  boolean_T success;

  // PrimitiveROSType=char
  uint8_T found_object[16];
  boolean_T reached_waypoint_without_detection;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_cmd_bus_
#define DEFINED_TYPEDEF_FOR_cmd_bus_

struct cmd_bus
{
  int8_T cmd_id[16];
  real_T wp[6];
  real_T wp_mask[6];
  real_T wp_tol[6];
  real_T hold_time;
  int8_T obj_id[16];
  real_T conf;
  int8_T trick_id[16];
  real_T exec_timeout;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Vector3_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Vector3_

// MsgType=geometry_msgs/Vector3
struct SL_Bus_geometry_msgs_Vector3
{
  real_T x;
  real_T y;
  real_T z;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_DRR_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_DRR_

// MsgType=custom_interfaces/DRR
struct SL_Bus_custom_interfaces_DRR
{
  // Int64Type=int64
  real_T time_stamp;

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 position;

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 angle;
  real32_T pos_std;
  uint8_T status;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_builtin_interfaces_Time_
#define DEFINED_TYPEDEF_FOR_SL_Bus_builtin_interfaces_Time_

// MsgType=builtin_interfaces/Time
struct SL_Bus_builtin_interfaces_Time
{
  int32_T sec;
  uint32_T nanosec;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_ROSVariableLengthArrayInfo_
#define DEFINED_TYPEDEF_FOR_SL_Bus_ROSVariableLengthArrayInfo_

struct SL_Bus_ROSVariableLengthArrayInfo
{
  uint32_T CurrentLength;
  uint32_T ReceivedLength;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Header_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Header_

// MsgType=std_msgs/Header
struct SL_Bus_std_msgs_Header
{
  // MsgType=builtin_interfaces/Time
  SL_Bus_builtin_interfaces_Time stamp;

  // PrimitiveROSType=string:IsVarLen=1:VarLenCategory=data:VarLenElem=frame_id_SL_Info:TruncateAction=warn 
  uint8_T frame_id[128];

  // IsVarLen=1:VarLenCategory=length:VarLenElem=frame_id
  SL_Bus_ROSVariableLengthArrayInfo frame_id_SL_Info;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Quaternion_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Quaternion_

// MsgType=geometry_msgs/Quaternion
struct SL_Bus_geometry_msgs_Quaternion
{
  real_T x;
  real_T y;
  real_T z;
  real_T w;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_Imu_
#define DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_Imu_

// MsgType=sensor_msgs/Imu
struct SL_Bus_sensor_msgs_Imu
{
  // MsgType=std_msgs/Header
  SL_Bus_std_msgs_Header header;

  // MsgType=geometry_msgs/Quaternion
  SL_Bus_geometry_msgs_Quaternion orientation;
  real_T orientation_covariance[9];

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 angular_velocity;
  real_T angular_velocity_covariance[9];

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 linear_acceleration;
  real_T linear_acceleration_covariance[9];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_MagneticField_
#define DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_MagneticField_

// MsgType=sensor_msgs/MagneticField
struct SL_Bus_sensor_msgs_MagneticField
{
  // MsgType=std_msgs/Header
  SL_Bus_std_msgs_Header header;

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 magnetic_field;
  real_T magnetic_field_covariance[9];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_FluidPressure_
#define DEFINED_TYPEDEF_FOR_SL_Bus_sensor_msgs_FluidPressure_

// MsgType=sensor_msgs/FluidPressure
struct SL_Bus_sensor_msgs_FluidPressure
{
  // MsgType=std_msgs/Header
  SL_Bus_std_msgs_Header header;
  real_T fluid_pressure;
  real_T variance;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Point_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Point_

// MsgType=geometry_msgs/Point
struct SL_Bus_geometry_msgs_Point
{
  real_T x;
  real_T y;
  real_T z;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Pose_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Pose_

// MsgType=geometry_msgs/Pose
struct SL_Bus_geometry_msgs_Pose
{
  // MsgType=geometry_msgs/Point
  SL_Bus_geometry_msgs_Point position;

  // MsgType=geometry_msgs/Quaternion
  SL_Bus_geometry_msgs_Quaternion orientation;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_PoseWithCovariance_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_PoseWithCovariance_

// MsgType=geometry_msgs/PoseWithCovariance
struct SL_Bus_geometry_msgs_PoseWithCovariance
{
  // MsgType=geometry_msgs/Pose
  SL_Bus_geometry_msgs_Pose pose;
  real_T covariance[36];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Twist_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_Twist_

// MsgType=geometry_msgs/Twist
struct SL_Bus_geometry_msgs_Twist
{
  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 linear;

  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 angular;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_TwistWithCovariance_
#define DEFINED_TYPEDEF_FOR_SL_Bus_geometry_msgs_TwistWithCovariance_

// MsgType=geometry_msgs/TwistWithCovariance
struct SL_Bus_geometry_msgs_TwistWithCovariance
{
  // MsgType=geometry_msgs/Twist
  SL_Bus_geometry_msgs_Twist twist;
  real_T covariance[36];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_nav_msgs_Odometry_
#define DEFINED_TYPEDEF_FOR_SL_Bus_nav_msgs_Odometry_

// MsgType=nav_msgs/Odometry
struct SL_Bus_nav_msgs_Odometry
{
  // MsgType=std_msgs/Header
  SL_Bus_std_msgs_Header header;

  // PrimitiveROSType=string:IsVarLen=1:VarLenCategory=data:VarLenElem=child_frame_id_SL_Info:TruncateAction=warn 
  uint8_T child_frame_id[128];

  // IsVarLen=1:VarLenCategory=length:VarLenElem=child_frame_id
  SL_Bus_ROSVariableLengthArrayInfo child_frame_id_SL_Info;

  // MsgType=geometry_msgs/PoseWithCovariance
  SL_Bus_geometry_msgs_PoseWithCovariance pose;

  // MsgType=geometry_msgs/TwistWithCovariance
  SL_Bus_geometry_msgs_TwistWithCovariance twist;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Imu_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Imu_

// MsgType=custom_interfaces/Imu
struct SL_Bus_custom_interfaces_Imu
{
  // MsgType=sensor_msgs/Imu
  SL_Bus_sensor_msgs_Imu imu_fusion;

  // MsgType=sensor_msgs/MagneticField
  SL_Bus_sensor_msgs_MagneticField mag_array;

  // MsgType=sensor_msgs/FluidPressure
  SL_Bus_sensor_msgs_FluidPressure pressure;

  // MsgType=nav_msgs/Odometry
  SL_Bus_nav_msgs_Odometry ahrs_database;
  real_T roll;
  real_T pitch;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_X_bus_
#define DEFINED_TYPEDEF_FOR_X_bus_

struct X_bus
{
  real_T Ri[3];
  real_T Rb[3];
  real_T Eul[3];
  real_T qib[4];
  real_T Cib[9];
  real_T Cbi[9];
  real_T dRi[3];
  real_T dRb[3];
  real_T wb[3];
  real_T wi[3];
  real_T ddRi[3];
  real_T ddRb[3];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_RSFF_maneuver_bus_
#define DEFINED_TYPEDEF_FOR_RSFF_maneuver_bus_

struct RSFF_maneuver_bus
{
  real_T rsff_stop[6];
  real_T rsff_forward[6];
  real_T rsff_backward[6];
  real_T rsff_right[6];
  real_T rsff_left[6];
  real_T rsff_up[6];
  real_T rsff_down[6];
  real_T rsff_rollRight[6];
  real_T rsff_rollLeft[6];
  real_T rsff_pitchUp[6];
  real_T rsff_pitchDown[6];
  real_T rsff_yawRight[6];
  real_T rsff_yawLeft[6];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_FF_maneuvers_bus_
#define DEFINED_TYPEDEF_FOR_FF_maneuvers_bus_

struct FF_maneuvers_bus
{
  real_T manual[8];
  real_T forward[8];
  real_T backward[8];
  real_T up[8];
  real_T down[8];
  real_T right[8];
  real_T left[8];
  real_T pitchUp[8];
  real_T pitchDown[8];
  real_T yawRight[8];
  real_T yawLeft[8];
  real_T rollRight[8];
  real_T rollLeft[8];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Debug_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Debug_

// MsgType=custom_interfaces/Debug
struct SL_Bus_custom_interfaces_Debug
{
  // PrimitiveROSType=char
  uint8_T message[256];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_MultiArrayDimension_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_MultiArrayDimension_

// MsgType=std_msgs/MultiArrayDimension
struct SL_Bus_std_msgs_MultiArrayDimension
{
  // PrimitiveROSType=string:IsVarLen=1:VarLenCategory=data:VarLenElem=label_SL_Info:TruncateAction=warn 
  uint8_T label[128];

  // IsVarLen=1:VarLenCategory=length:VarLenElem=label
  SL_Bus_ROSVariableLengthArrayInfo label_SL_Info;
  uint32_T size;
  uint32_T stride;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_MultiArrayLayout_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_MultiArrayLayout_

// MsgType=std_msgs/MultiArrayLayout
struct SL_Bus_std_msgs_MultiArrayLayout
{
  // MsgType=std_msgs/MultiArrayDimension:IsVarLen=1:VarLenCategory=data:VarLenElem=dim_SL_Info:TruncateAction=warn 
  SL_Bus_std_msgs_MultiArrayDimension dim[16];

  // IsVarLen=1:VarLenCategory=length:VarLenElem=dim
  SL_Bus_ROSVariableLengthArrayInfo dim_SL_Info;
  uint32_T data_offset;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Float32MultiArray_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Float32MultiArray_

// MsgType=std_msgs/Float32MultiArray
struct SL_Bus_std_msgs_Float32MultiArray
{
  // MsgType=std_msgs/MultiArrayLayout
  SL_Bus_std_msgs_MultiArrayLayout layout;

  // IsVarLen=1:VarLenCategory=data:VarLenElem=data_SL_Info:TruncateAction=warn
  real32_T data[128];

  // IsVarLen=1:VarLenCategory=length:VarLenElem=data
  SL_Bus_ROSVariableLengthArrayInfo data_SL_Info;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_VR_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_VR_

// MsgType=custom_interfaces/VR
struct SL_Bus_custom_interfaces_VR
{
  // MsgType=geometry_msgs/Vector3
  SL_Bus_geometry_msgs_Vector3 velocity_data;
  real32_T altitude;
  real32_T fom;
  real32_T time;

  // MsgType=std_msgs/Float32MultiArray
  SL_Bus_std_msgs_Float32MultiArray covariance;

  // Int64Type=int64
  real_T time_of_validity;

  // Int64Type=int64
  real_T time_of_transmission;
  uint8_T status;

  // PrimitiveROSType=char
  uint8_T valid;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Bool_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Bool_

// MsgType=std_msgs/Bool
struct SL_Bus_std_msgs_Bool
{
  boolean_T data;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Gamepad_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Gamepad_

// MsgType=custom_interfaces/Gamepad
struct SL_Bus_custom_interfaces_Gamepad
{
  real32_T x;
  real32_T y;
  real32_T rise;
  real32_T sink;
  real32_T yaw;
  real32_T pitch;
  boolean_T cross_button;
  boolean_T square_button;
  boolean_T triangle_button;
  boolean_T circle_button;
  boolean_T dpad_down;
  boolean_T dpad_left;
  boolean_T dpad_up;
  boolean_T dpad_right;
  boolean_T bumper_left;
  boolean_T bumper_right;
  boolean_T start;
  boolean_T select;
  boolean_T joystick_press_left;
  boolean_T joystick_press_right;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Empty_
#define DEFINED_TYPEDEF_FOR_SL_Bus_std_msgs_Empty_

// MsgType=std_msgs/Empty
struct SL_Bus_std_msgs_Empty
{
  boolean_T SL_DummyData;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Pwms_
#define DEFINED_TYPEDEF_FOR_SL_Bus_custom_interfaces_Pwms_

// MsgType=custom_interfaces/Pwms
struct SL_Bus_custom_interfaces_Pwms
{
  int32_T pwms[8];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_sensor_bus_
#define DEFINED_TYPEDEF_FOR_sensor_bus_

struct sensor_bus
{
  real_T dvl_std;
  real_T dvl_eul[3];
  real_T dvl_pos[3];
  real_T dvl_vel[3];
  real_T dvl_alt;
  real_T dvl_cov[9];
  real_T dvl_fom;
  real_T imu_pose_cov[9];
  real_T imu_pose_pos[3];
  real_T imu_pos_quat[4];
  real_T imu_twist_cov[3];
  real_T imu_twist_ang[3];
  real_T imu_twist_lin[3];
  real_T imu_ang_vel_cov[9];
  real_T imu_ang_vel[3];
  real_T imu_lin_acc_cov[9];
  real_T imu_lin_acc[3];
  real_T imu_quat_cov[16];
  real_T imu_quat[4];
  real_T imu_mag[3];
  real_T imu_mag_cov[9];
  real_T imu_new_msg_flag;
  real_T dvl_vr_new_msg_flag;
  real_T dvl_drr_new_msg_flag;
};

#endif

// Custom Type definition for MATLABSystem: '<S229>/SinkBlock'
#include "rmw/qos_profiles.h"
#ifndef struct_sJ4ih70VmKcvCeguWN0mNVF
#define struct_sJ4ih70VmKcvCeguWN0mNVF

struct sJ4ih70VmKcvCeguWN0mNVF
{
  real_T sec;
  real_T nsec;
};

#endif                                 // struct_sJ4ih70VmKcvCeguWN0mNVF

#ifndef struct_ros_slros2_internal_block_Sub_T
#define struct_ros_slros2_internal_block_Sub_T

struct ros_slros2_internal_block_Sub_T
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  boolean_T QOSAvoidROSNamespaceConventions;
};

#endif                                // struct_ros_slros2_internal_block_Sub_T

#ifndef struct_ros_slros2_internal_block_Pub_T
#define struct_ros_slros2_internal_block_Pub_T

struct ros_slros2_internal_block_Pub_T
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  boolean_T QOSAvoidROSNamespaceConventions;
};

#endif                                // struct_ros_slros2_internal_block_Pub_T

// Custom Type definition for MATLAB Function: '<S6>/eulToQuat'
#ifndef struct_cell_wrap_0_matlab_control_T
#define struct_cell_wrap_0_matlab_control_T

struct cell_wrap_0_matlab_control_T
{
  char_T f1[16];
};

#endif                                 // struct_cell_wrap_0_matlab_control_T

// Forward declaration for rtModel
typedef struct tag_RTM_matlab_control_T RT_MODEL_matlab_control_T;

#endif                                 // matlab_control_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
