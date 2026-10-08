//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: Simple_Joystick_Controller_types.h
//
// Code generated for Simulink model 'Simple_Joystick_Controller'.
//
// Model version                  : 1.16
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu May 14 23:42:13 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: Intel->x86-64 (Linux 64)
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef Simple_Joystick_Controller_types_h_
#define Simple_Joystick_Controller_types_h_
#include "rtwtypes.h"
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

// Custom Type definition for MATLABSystem: '<S12>/SinkBlock'
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

// Forward declaration for rtModel
typedef struct tag_RTM_Simple_Joystick_Contr_T RT_MODEL_Simple_Joystick_Cont_T;

#endif                                 // Simple_Joystick_Controller_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
