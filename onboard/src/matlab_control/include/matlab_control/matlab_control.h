//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: matlab_control.h
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
#ifndef matlab_control_h_
#define matlab_control_h_
#include <cmath>
#include <cstring>
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "slros2_initialize.h"
#include "matlab_control_types.h"

extern "C"
{

#include "rtGetNaN.h"

}

// Block signals for system '<S25>/quatToEul'
struct B_quatToEul_matlab_control_T {
  real_T Cbi[9];
  real_T a[9];
  real_T b_a[9];
};

// Block signals (default storage)
struct B_matlab_control_T {
  SL_Bus_custom_interfaces_VR In1;     // '<S220>/In1'
  SL_Bus_custom_interfaces_VR rtb_SourceBlock_o2_m;
  SL_Bus_custom_interfaces_Imu In1_h;  // '<S24>/In1'
  SL_Bus_custom_interfaces_Imu rtb_SourceBlock_o2_l_c;
  real_T force_column_data[201];
  real_T b_varargin_1_data[201];
  real_T b_x_data[201];
  X_bus X_est;                         // '<S6>/derivedStateEstimates'
  char_T temp_char[256];
  SL_Bus_custom_interfaces_Debug debug_msg;// '<S7>/MATLAB Function'
  cmd_bus cmd;                         // '<S1>/cmdMsgToCmdBus'
  cell_wrap_0_matlab_control_T varargin_1[13];
  cell_wrap_0_matlab_control_T varargin_2[13];
  cell_wrap_0_matlab_control_T varargin_3[13];
  SL_Bus_custom_interfaces_Goal In1_p; // '<S21>/In1'
  real_T TmpSignalConversionAtSFunct[22];// '<S7>/MATLAB Function'
  SL_Bus_custom_interfaces_Goal rtb_SourceBlock_o2_n_k;
  real_T c_a[16];
  real_T X_u[13];                      // '<S26>/commandExecuter'
  real_T Qe1[12];
  real_T Cbi[9];
  real_T rtb_sensors_dvl_cov[9];
  real_T Cbi_c[9];                     // '<S6>/Transpose'
  real_T qk_plus_1_tmp[9];
  real_T rtb_Cbi_tmp[9];
  real_T rtb_Cbi_tmp_b[9];
  real_T Cbi_p[9];
  real_T a[9];
  real_T b_a[9];
  SL_Bus_custom_interfaces_DRR In1_k;  // '<S23>/In1'
  real_T pwms[8];
  real_T FT_cmd_list[8];               // '<S25>/thrusterAllocator'
  real_T dv[8];
  SL_Bus_custom_interfaces_DRR rtb_SourceBlock_o2_j_c;
  real_T varargin_1_f[6];
  real_T R_error[6];
  SL_Bus_custom_interfaces_Gamepad In1_o;// '<S225>/In1'
  SL_Bus_custom_interfaces_Gamepad rtb_SourceBlock_o2_g_g;
  SL_Bus_custom_interfaces_Pwms BusCreator3;// '<S14>/Bus Creator3'
  real_T qinv[4];
  real_T qe_matlab[4];
  real_T qe_matlab_g[4];
  real_T qib_int_u[4];                 // '<S25>/guidanceLaw'
  real_T dv1[4];
  real_T DiscreteTransferFcn[4];
  real_T dv2[4];
  real_T qinv_m[4];
  real_T eps[3];
  real_T eps_n[3];
  real_T eps_p[3];
  real_T Rb_u[3];                      // '<S25>/guidanceLaw'
  real_T wb_u[3];                      // '<S25>/quatPID'
  real_T dRb_e[3];                     // '<S25>/rateInjector'
  real_T Integrator_j[3];              // '<S191>/Integrator'
  real_T dv3[3];
  real_T rtb_dvl_pos_l[3];
  real_T dv4[3];
  real_T x[3];
  real_T x_j[3];
  char_T b_zeroDelimTopic[18];
  char_T b_zeroDelimTopic_d[18];
  SL_Bus_custom_interfaces_Result result_msg;// '<S1>/statusKwdToBool'
  char_T b_zeroDelimTopic_g[17];
  char_T x1[16];
  char_T s[16];
  cell_wrap_0_matlab_control_T b;
  cell_wrap_0_matlab_control_T c;
  cell_wrap_0_matlab_control_T d;
  cell_wrap_0_matlab_control_T e;
  cell_wrap_0_matlab_control_T f;
  cell_wrap_0_matlab_control_T g;
  cell_wrap_0_matlab_control_T h;
  cell_wrap_0_matlab_control_T i;
  cell_wrap_0_matlab_control_T j;
  cell_wrap_0_matlab_control_T k;
  cell_wrap_0_matlab_control_T l;
  cell_wrap_0_matlab_control_T m;
  cell_wrap_0_matlab_control_T n;
  cell_wrap_0_matlab_control_T o;
  cell_wrap_0_matlab_control_T p;
  cell_wrap_0_matlab_control_T q;
  cell_wrap_0_matlab_control_T r;
  cell_wrap_0_matlab_control_T s_l;
  cell_wrap_0_matlab_control_T u;
  cell_wrap_0_matlab_control_T v;
  cell_wrap_0_matlab_control_T w;
  cell_wrap_0_matlab_control_T x_d;
  cell_wrap_0_matlab_control_T y;
  cell_wrap_0_matlab_control_T ab;
  cell_wrap_0_matlab_control_T bb;
  cell_wrap_0_matlab_control_T cb;
  char_T switch_expression[16];
  char_T b_zeroDelimTopic_dy[16];
  char_T b_zeroDelimTopic_l[16];
  char_T b_zeroDelimTopic_o[16];
  sJ4ih70VmKcvCeguWN0mNVF deadline;
  sJ4ih70VmKcvCeguWN0mNVF deadline_b;
  sJ4ih70VmKcvCeguWN0mNVF deadline_n;
  sJ4ih70VmKcvCeguWN0mNVF deadline_bs;
  real_T Ri_xy_e[2];
  char_T digits[10];
  real_T eta;
  real_T scale;
  real_T absxk;
  real_T b_t;
  real_T norm_w;
  real_T b_a_l;
  real_T t;
  real_T b_scale;
  real_T b_absxk;
  real_T theta;
  real_T theta_h;
  real_T lower_voltage_index;
  real_T upper_voltage_index;
  real_T upper_force_index;
  real_T alpha;
  real_T phi;
  real_T idx;
  real_T rotm_tmp;
  real_T eps_b;
  real_T rtb_Integrator_j_d;
  real_T rtb_delta_qe_idx_2;
  real_T rtb_delta_qe_idx_1;
  real_T rtb_delta_qe_idx_3;
  real_T pwms_horiz_idx_1;
  real_T pwms_horiz_idx_2;
  real_T pwms_horiz_idx_3;
  real_T rtb_Eul_idx_0;
  real_T DiscreteTransferFcn_tmp_idx_2;
  real_T rtb_Filter_d_idx_0;
  real_T rtb_X_est_idx_2;
  real_T Qe1_e;
  real_T rtb_Rb_u_b;
  real_T rtb_Rb_u_j;
  real_T eps_f;
  real_T rtb_X_est_wb;
  real_T rtb_X_est_wb_a;
  real_T eps_tmp;
  real_T rtb_delta_qe_idx_1_tmp;
  real_T eta_j;
  real_T scale_j;
  real_T absxk_o;
  real_T b_t_n;
  real_T rotm_tmp_i;
  real_T rotm_tmp_o;
  real_T eps_idx_0;
  real_T eps_idx_1;
  real_T eps_idx_2;
  real_T rotm_idx_0;
  real_T rotm_idx_8;
  real_T rotm_idx_4_tmp;
  real_T qe_matlab_idx_0;
  real_T qe_matlab_idx_1;
  real_T qe_matlab_idx_2;
  real_T qe_matlab_idx_3;
  real_T varargin_1_data;
  real_T q_n;
  real_T eta_m;
  real_T scale_c;
  real_T absxk_m;
  real_T b_t_m;
  real_T rotm_tmp_j;
  real_T rotm_tmp_h;
  real_T eps_idx_0_c;
  real_T eps_idx_1_c;
  real_T eps_idx_2_p;
  real_T rotm_idx_0_p;
  real_T rotm_idx_8_a;
  real_T rotm_idx_4_tmp_e;
  real_T scale_a;
  real_T absxk_a;
  real_T t_i;
  real_T x_l;
  real_T x_o;
  SL_Bus_std_msgs_Bool In1_m;          // '<S222>/In1'
  boolean_T b_x[6];
  char_T fd[4];
  int32_T k_o;
  int32_T d_k;
  int32_T pos;
  int32_T db6;
  int32_T i_i;
  int32_T b_varargin_1_size;
  int32_T force_column_size_idx_0;
  int32_T c_ret;
  int32_T d_ret;
  int32_T idx_f;
  int32_T b_k;
  int32_T c_k;
  int32_T c_k_i;
  int32_T trueCount;
  int32_T i_f;
  int32_T i_g;
  int32_T idx_c;
  int32_T b_k_o;
  int32_T c_k_l;
  int32_T b_idx;
  int32_T last;
  int32_T k_m;
  int32_T b_idx_m;
  int32_T k_c;
  int32_T i1;
  int32_T i2;
  int32_T k_f;
  int32_T k_p;
  int32_T k_e;
  int32_T k_o4;
  int32_T k_h;
  int32_T k_l;
  int32_T ret;
  int32_T ret_h;
  int32_T ret_m;
  uint32_T int_part;
  uint32_T frac_part;
  uint32_T temp;
  boolean_T new_waypoint_latch;
  boolean_T b_varargout_1;
  boolean_T b_varargout_1_m;
  boolean_T b_varargout_1_h;
  boolean_T DataStoreRead_m;           // '<S1>/Data Store Read'
  boolean_T reset_dvl_pos;
  boolean_T reset_pids;
  boolean_T rtb_fixforDTpropagationissue_id;
  SL_Bus_std_msgs_Bool rtb_SourceBlock_o2_h_k;
  B_quatToEul_matlab_control_T sf_quatToEul1;// '<S25>/quatToEul1'
  B_quatToEul_matlab_control_T sf_quatToEul;// '<S25>/quatToEul'
};

// Block states (default storage) for system '<Root>'
struct DW_matlab_control_T {
  ros_slros2_internal_block_Sub_T obj; // '<S224>/SourceBlock'
  ros_slros2_internal_block_Sub_T obj_a;// '<S221>/SourceBlock'
  ros_slros2_internal_block_Sub_T obj_e;// '<S8>/SourceBlock'
  ros_slros2_internal_block_Sub_T obj_j;// '<S3>/SourceBlock'
  ros_slros2_internal_block_Sub_T obj_o;// '<S2>/SourceBlock'
  ros_slros2_internal_block_Sub_T obj_f;// '<S17>/SourceBlock'
  ros_slros2_internal_block_Pub_T obj_jp;// '<S229>/SinkBlock'
  ros_slros2_internal_block_Pub_T obj_p;// '<S228>/SinkBlock'
  ros_slros2_internal_block_Pub_T obj_n;// '<S219>/SinkBlock'
  ros_slros2_internal_block_Pub_T obj_d;// '<S22>/SinkBlock'
  real_T PositionIntegrator_DSTATE[3]; // '<S6>/Position Integrator'
  real_T VelocityIntegrator_DSTATE[3]; // '<S6>/Velocity Integrator'
  real_T UnitDelay_DSTATE;             // '<S4>/Unit Delay'
  real_T UnitDelay1_DSTATE;            // '<S4>/Unit Delay1'
  real_T DelayInput1_DSTATE;           // '<S29>/Delay Input1'
  real_T DiscreteTimeIntegrator_DSTATE[4];// '<S31>/Discrete-Time Integrator'
  real_T DiscreteTransferFcn_states[4];// '<S25>/Discrete Transfer Fcn'
  real_T Integrator_DSTATE[3];         // '<S136>/Integrator'
  real_T Filter_DSTATE[3];             // '<S131>/Filter'
  real_T Integrator_DSTATE_e[3];       // '<S81>/Integrator'
  real_T Filter_DSTATE_g[3];           // '<S76>/Filter'
  real_T Integrator_DSTATE_f[3];       // '<S191>/Integrator'
  real_T Filter_DSTATE_f[3];           // '<S186>/Filter'
  real_T db11;                         // '<Root>/Data Store Memory15'
  real_T offset[3];                    // '<S6>/dvl_reset'
  real_T qk[4];                        // '<S6>/discreteTimeQuatPropagation'
  real_T hold_timer_start_time;        // '<S26>/commandExecuter'
  real_T idle_wp[6];                   // '<S26>/commandExecuter'
  real_T cmd_specific_wp[6];           // '<S26>/commandExecuter'
  real_T prior_action_id;              // '<S26>/commandExecuter'
  real_T body_relative_wp[3];          // '<S26>/commandExecuter'
  real_T persistant_yaw_target;        // '<S25>/guidanceLaw'
  real_T action_id;                    // '<S25>/guidanceLaw'
  real_T prior_action_id_c;            // '<S25>/guidanceLaw'
  real_T prior_Ri_u[3];                // '<S25>/guidanceLaw'
  int8_T UnitDelay_DSTATE_k[4];        // '<S1>/Unit Delay'
  int8_T PositionIntegrator_PrevResetSta;// '<S6>/Position Integrator'
  int8_T VelocityIntegrator_PrevResetSta;// '<S6>/Velocity Integrator'
  int8_T DiscreteTimeIntegrator_PrevRese;// '<S31>/Discrete-Time Integrator'
  int8_T Integrator_PrevResetState;    // '<S136>/Integrator'
  int8_T Filter_PrevResetState;        // '<S131>/Filter'
  int8_T Integrator_PrevResetState_o;  // '<S81>/Integrator'
  int8_T Filter_PrevResetState_m;      // '<S76>/Filter'
  int8_T Integrator_PrevResetState_h;  // '<S191>/Integrator'
  int8_T Filter_PrevResetState_n;      // '<S186>/Filter'
  uint8_T PositionIntegrator_IC_LOADING;// '<S6>/Position Integrator'
  uint8_T VelocityIntegrator_IC_LOADING;// '<S6>/Velocity Integrator'
  boolean_T mission_is_started;        // '<Root>/Data Store Memory3'
  boolean_T joystick_flag;             // '<Root>/joystick_mode_manager'
  boolean_T joystick_flag_not_empty;   // '<Root>/joystick_mode_manager'
  boolean_T prior_joystick_flag;       // '<Root>/joystick_mode_manager'
  boolean_T prior_joystick_flag_not_empty;// '<Root>/joystick_mode_manager'
  boolean_T do_mission_file_flag;      // '<Root>/joystick_mode_manager'
  boolean_T do_mission_file_flag_not_empty;// '<Root>/joystick_mode_manager'
  boolean_T prior_do_mission_file_flag;// '<Root>/joystick_mode_manager'
  boolean_T prior_do_mission_file_flag_not_;// '<Root>/joystick_mode_manager'
  boolean_T prior_flag;                // '<S6>/dvl_reset'
  boolean_T qk_not_empty;              // '<S6>/discreteTimeQuatPropagation'
  boolean_T hold_timer_start_time_not_empty;// '<S26>/commandExecuter'
  boolean_T idle_wp_not_empty;         // '<S26>/commandExecuter'
  boolean_T cmd_specific_wp_not_empty; // '<S26>/commandExecuter'
  boolean_T wp_defined_latch;          // '<S26>/commandExecuter'
  boolean_T prior_action_id_not_empty; // '<S25>/guidanceLaw'
};

// Constant parameters (default storage)
struct ConstP_matlab_control_T {
  // Expression: FF_maneuvers
  //  Referenced by: '<S25>/trickFTListInjector'

  FF_maneuvers_bus trickFTListInjector_FF_maneuver;

  // Computed Parameter: Constant_Value_i
  //  Referenced by: '<S227>/Constant'

  SL_Bus_std_msgs_Empty Constant_Value_i;

  // Expression: ccw_force
  //  Referenced by: '<S32>/forceToPWMCalculator'

  real_T forceToPWMCalculator_ccw_force[1206];

  // Expression: ccw_pwm
  //  Referenced by: '<S32>/forceToPWMCalculator'

  real_T forceToPWMCalculator_ccw_pwm[201];

  // Expression: cw_force
  //  Referenced by: '<S32>/forceToPWMCalculator'

  real_T forceToPWMCalculator_cw_force[1206];

  // Expression: cw_pwm
  //  Referenced by: '<S32>/forceToPWMCalculator'

  real_T forceToPWMCalculator_cw_pwm[201];

  // Expression: voltage
  //  Referenced by: '<S32>/forceToPWMCalculator'

  real_T forceToPWMCalculator_voltage[6];

  // Expression: invMT_wrench
  //  Referenced by: '<S25>/Constant'

  real_T Constant_Value_n[24];

  // Expression: invFT_wrench
  //  Referenced by: '<S25>/Constant1'

  real_T Constant1_Value[24];

  // Expression: gi
  //  Referenced by: '<S6>/Constant'

  real_T Constant_Value_k[3];

  // Expression: Cbimu_meas
  //  Referenced by: '<S6>/Constant1'

  real_T Constant1_Value_a[9];

  // Expression: qib_PID.Kp
  //  Referenced by: '<S25>/Gain1'

  real_T Gain1_Gain[4];

  // Expression: P
  //  Referenced by: '<S141>/Proportional Gain'

  real_T ProportionalGain_Gain[3];
};

// Real-time Model Data Structure
struct tag_RTM_matlab_control_T {
  const char_T * volatile errorStatus;

  //
  //  Timing:
  //  The following substructure contains information regarding
  //  the timing information for the model.

  struct {
    uint32_T clockTick0;
    struct {
      uint8_T TID[2];
    } TaskCounters;
  } Timing;

  uint8_T &TaskCounter(int32_T idx);
  boolean_T StepTask(int32_T idx) const;
  const char_T* getErrorStatus() const;
  void setErrorStatus(const char_T* const volatile aErrorStatus);
};

// Constant parameters (default storage)
extern const ConstP_matlab_control_T matlab_control_ConstP;

// External function called from main
#ifdef __cplusplus

extern "C"
{

#endif

  extern void matlab_control_SetEventsForThisBaseStep(boolean_T *eventFlags,
    RT_MODEL_matlab_control_T *const matlab_control_M);

#ifdef __cplusplus

}

#endif

// Class declaration for model matlab_control
class matlab_control final
{
  // public data and function members
 public:
  // Copy Constructor
  matlab_control(matlab_control const&) = delete;

  // Assignment Operator
  matlab_control& operator= (matlab_control const&) & = delete;

  // Move Constructor
  matlab_control(matlab_control &&) = delete;

  // Move Assignment Operator
  matlab_control& operator= (matlab_control &&) = delete;

  // Real-Time Model get method
  RT_MODEL_matlab_control_T * getRTM();

  // model initialize function
  void initialize();

  // model step function
  void step0();

  // model step function
  void step1();

  // model terminate function
  void terminate();

  // Constructor
  matlab_control();

  // Destructor
  ~matlab_control();

  // private data and function members
 private:
  // Block signals
  B_matlab_control_T matlab_control_B;

  // Block states
  DW_matlab_control_T matlab_control_DW;

  // private member function(s) for subsystem '<S25>/quatToEul'
  void matlab_control_quatToEul(const real_T rtu_qib_u[4], real_T rty_Eul_u[3],
    B_quatToEul_matlab_control_T *localB);
  void matlab_control_wrapToPi(real_T *lambda);

  // private member function(s) for subsystem '<Root>'
  real_T matlab_control_rt_atan2d_snf(real_T u0, real_T u1);
  void matlab_control_wrapToPi_h(real_T *lambda);
  void matlab_control_eulToQuat_l(const real_T Eul[3], real_T qib[4]);
  boolean_T matlab_control_withinWPTol(const real_T X_Ri[3], const real_T X_qib
    [4], const real_T X_u[13], const real_T cmd_wp_tol[6]);
  void matlab_con_executeDurationTrick(real_T cmd_hold_time, const int8_T
    cmd_trick_id[16], const real_T idle_wp[6], const real_T X_Ri[3], const
    real_T X_Eul[3], const real_T X_Cib[9], real_T *hold_timer_start_time,
    real_T t, boolean_T new_cmd_reset, real_T cmd_specific_wp[6], int8_T
    cmd_status[4], real_T *hold_timer, real_T X_u[13]);
  real_T matlab_control_norm(const real_T x[2]);
  void matlab_control_eulToQuat(const real_T Eul[3], real_T qib[4]);
  boolean_T matlab_control_isequal(const char_T varargin_1[16]);
  boolean_T matlab_control_strcmp(const char_T b[16]);
  boolean_T matlab_control_strcmp_n(const char_T b[16]);
  boolean_T matlab_control_strcmp_n3(const char_T b[16]);
  boolean_T matlab_control_isequal_oi(const char_T varargin_1[16]);
  boolean_T matlab_control_isequal_oia(const char_T varargin_1[16]);
  boolean_T matlab_control_isequal_oia2(const char_T varargin_1[16]);
  boolean_T matlab_control_isequal_oia20(const char_T varargin_1[16]);
  boolean_T matlab_control_isequal_oia20k(const char_T varargin_1[16]);
  void matlab_control_minimum(const real_T x[6], real_T *ex, int32_T *idx);
  void matlab_control_minimum_f(const real_T x_data[], const int32_T *x_size,
    real_T *ex, int32_T *idx);
  void matlab_cont_Publisher_setupImpl(const ros_slros2_internal_block_Pub_T
    *obj);
  void matl_Subscriber_setupImpl_nsnn0(const ros_slros2_internal_block_Sub_T
    *obj);
  void matlab__Subscriber_setupImpl_ns(const ros_slros2_internal_block_Sub_T
    *obj);
  void matlab_Subscriber_setupImpl_nsn(const ros_slros2_internal_block_Sub_T
    *obj);
  void matlab_c_Subscriber_setupImpl_n(const ros_slros2_internal_block_Sub_T
    *obj);
  void matlab_con_Subscriber_setupImpl(const ros_slros2_internal_block_Sub_T
    *obj);
  void matla_Subscriber_setupImpl_nsnn(const ros_slros2_internal_block_Sub_T
    *obj);
  void matlab__Publisher_setupImpl_nsn(const ros_slros2_internal_block_Pub_T
    *obj);
  void matlab_co_Publisher_setupImpl_n(const ros_slros2_internal_block_Pub_T
    *obj);
  void matlab_c_Publisher_setupImpl_ns(const ros_slros2_internal_block_Pub_T
    *obj);

  // Real-Time Model
  RT_MODEL_matlab_control_T matlab_control_M;
};

extern volatile boolean_T stopRequested;
extern volatile boolean_T runModel;

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S1>/Constant' : Unused code path elimination
//  Block '<S1>/Scope' : Unused code path elimination
//  Block '<S25>/Discrete-Time Integrator' : Unused code path elimination
//  Block '<S25>/Discrete-Time Integrator1' : Unused code path elimination
//  Block '<S25>/Scope' : Unused code path elimination
//  Block '<S25>/Scope1' : Unused code path elimination
//  Block '<S25>/Scope4' : Unused code path elimination
//  Block '<S6>/Display' : Unused code path elimination
//  Block '<S6>/Display1' : Unused code path elimination
//  Block '<S6>/Display2' : Unused code path elimination
//  Block '<S6>/Scope' : Unused code path elimination
//  Block '<S25>/Gain' : Eliminated nontunable gain of 1
//  Block '<S25>/Gain3' : Eliminated nontunable gain of 1
//  Block '<S25>/Gain4' : Eliminated nontunable gain of 1


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'matlab_control'
//  '<S1>'   : 'matlab_control/ '
//  '<S2>'   : 'matlab_control/DRR'
//  '<S3>'   : 'matlab_control/IMU'
//  '<S4>'   : 'matlab_control/Low-Level Controller'
//  '<S5>'   : 'matlab_control/MATLAB Function'
//  '<S6>'   : 'matlab_control/Subsystem Reference'
//  '<S7>'   : 'matlab_control/Subsystem Reference1'
//  '<S8>'   : 'matlab_control/VR'
//  '<S9>'   : 'matlab_control/dvl_reset_subscriber'
//  '<S10>'  : 'matlab_control/joystick_mode_manager'
//  '<S11>'  : 'matlab_control/joystick_output'
//  '<S12>'  : 'matlab_control/joystick_to_pwm'
//  '<S13>'  : 'matlab_control/pwm_heartbeat_publisher'
//  '<S14>'  : 'matlab_control/pwm_publisher'
//  '<S15>'  : 'matlab_control/sensorRosMsgToBus'
//  '<S16>'  : 'matlab_control/ /MATLAB Function'
//  '<S17>'  : 'matlab_control/ /Subscribe'
//  '<S18>'  : 'matlab_control/ /Subsystem'
//  '<S19>'  : 'matlab_control/ /cmdMsgToCmdBus'
//  '<S20>'  : 'matlab_control/ /statusKwdToBool'
//  '<S21>'  : 'matlab_control/ /Subscribe/Enabled Subsystem'
//  '<S22>'  : 'matlab_control/ /Subsystem/Publish'
//  '<S23>'  : 'matlab_control/DRR/Enabled Subsystem'
//  '<S24>'  : 'matlab_control/IMU/Enabled Subsystem'
//  '<S25>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller'
//  '<S26>'  : 'matlab_control/Low-Level Controller/execute_command'
//  '<S27>'  : 'matlab_control/Low-Level Controller/quatToEul'
//  '<S28>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller'
//  '<S29>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Detect Change'
//  '<S30>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb'
//  '<S31>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Subsystem'
//  '<S32>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Subsystem Reference1'
//  '<S33>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller'
//  '<S34>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRbSetpointInjector'
//  '<S35>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/deltaQuatError'
//  '<S36>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/guidanceLaw'
//  '<S37>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/quatInjector'
//  '<S38>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/quatPID'
//  '<S39>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/quatToEul'
//  '<S40>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/quatToEul1'
//  '<S41>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/quatToRotm'
//  '<S42>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/rateInjector'
//  '<S43>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/thrusterAllocator'
//  '<S44>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/trickFTListInjector'
//  '<S45>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Anti-windup'
//  '<S46>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/D Gain'
//  '<S47>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/External Derivative'
//  '<S48>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Filter'
//  '<S49>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Filter ICs'
//  '<S50>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/I Gain'
//  '<S51>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Ideal P Gain'
//  '<S52>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Ideal P Gain Fdbk'
//  '<S53>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Integrator'
//  '<S54>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Integrator ICs'
//  '<S55>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/N Copy'
//  '<S56>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/N Gain'
//  '<S57>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/P Copy'
//  '<S58>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Parallel P Gain'
//  '<S59>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Reset Signal'
//  '<S60>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Saturation'
//  '<S61>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Saturation Fdbk'
//  '<S62>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Sum'
//  '<S63>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Sum Fdbk'
//  '<S64>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tracking Mode'
//  '<S65>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tracking Mode Sum'
//  '<S66>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tsamp - Integral'
//  '<S67>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tsamp - Ngain'
//  '<S68>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/postSat Signal'
//  '<S69>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/preInt Signal'
//  '<S70>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/preSat Signal'
//  '<S71>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S72>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S73>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S74>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/D Gain/Internal Parameters'
//  '<S75>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/External Derivative/Error'
//  '<S76>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Filter/Disc. Forward Euler Filter'
//  '<S77>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Filter ICs/Internal IC - Filter'
//  '<S78>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/I Gain/Internal Parameters'
//  '<S79>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Ideal P Gain/Passthrough'
//  '<S80>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Ideal P Gain Fdbk/Disabled'
//  '<S81>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Integrator/Discrete'
//  '<S82>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Integrator ICs/Internal IC'
//  '<S83>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/N Copy/Disabled'
//  '<S84>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/N Gain/Internal Parameters'
//  '<S85>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/P Copy/Disabled'
//  '<S86>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Parallel P Gain/Internal Parameters'
//  '<S87>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Reset Signal/External Reset'
//  '<S88>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Saturation/Enabled'
//  '<S89>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Saturation Fdbk/Disabled'
//  '<S90>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Sum/Sum_PID'
//  '<S91>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Sum Fdbk/Disabled'
//  '<S92>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tracking Mode/Disabled'
//  '<S93>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tracking Mode Sum/Passthrough'
//  '<S94>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S95>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/Tsamp - Ngain/Passthrough'
//  '<S96>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/postSat Signal/Forward_Path'
//  '<S97>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/preInt Signal/Internal PreInt'
//  '<S98>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Angular Velocity Controller/preSat Signal/Forward_Path'
//  '<S99>'  : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller'
//  '<S100>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Anti-windup'
//  '<S101>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/D Gain'
//  '<S102>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/External Derivative'
//  '<S103>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Filter'
//  '<S104>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Filter ICs'
//  '<S105>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/I Gain'
//  '<S106>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Ideal P Gain'
//  '<S107>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Ideal P Gain Fdbk'
//  '<S108>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Integrator'
//  '<S109>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Integrator ICs'
//  '<S110>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/N Copy'
//  '<S111>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/N Gain'
//  '<S112>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/P Copy'
//  '<S113>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Parallel P Gain'
//  '<S114>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Reset Signal'
//  '<S115>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Saturation'
//  '<S116>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Saturation Fdbk'
//  '<S117>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Sum'
//  '<S118>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Sum Fdbk'
//  '<S119>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tracking Mode'
//  '<S120>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tracking Mode Sum'
//  '<S121>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tsamp - Integral'
//  '<S122>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tsamp - Ngain'
//  '<S123>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/postSat Signal'
//  '<S124>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/preInt Signal'
//  '<S125>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/preSat Signal'
//  '<S126>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S127>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S128>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S129>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/D Gain/Internal Parameters'
//  '<S130>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/External Derivative/Error'
//  '<S131>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Filter/Disc. Forward Euler Filter'
//  '<S132>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Filter ICs/Internal IC - Filter'
//  '<S133>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/I Gain/Internal Parameters'
//  '<S134>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Ideal P Gain/Passthrough'
//  '<S135>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S136>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Integrator/Discrete'
//  '<S137>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Integrator ICs/Internal IC'
//  '<S138>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/N Copy/Disabled'
//  '<S139>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/N Gain/Internal Parameters'
//  '<S140>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/P Copy/Disabled'
//  '<S141>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S142>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Reset Signal/External Reset'
//  '<S143>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Saturation/Enabled'
//  '<S144>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Saturation Fdbk/Disabled'
//  '<S145>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Sum/Sum_PID'
//  '<S146>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Sum Fdbk/Disabled'
//  '<S147>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tracking Mode/Disabled'
//  '<S148>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tracking Mode Sum/Passthrough'
//  '<S149>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S150>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S151>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/postSat Signal/Forward_Path'
//  '<S152>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/preInt Signal/Internal PreInt'
//  '<S153>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/PID for Rb/PID Controller/preSat Signal/Forward_Path'
//  '<S154>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/Subsystem Reference1/forceToPWMCalculator'
//  '<S155>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Anti-windup'
//  '<S156>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/D Gain'
//  '<S157>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/External Derivative'
//  '<S158>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Filter'
//  '<S159>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Filter ICs'
//  '<S160>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/I Gain'
//  '<S161>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Ideal P Gain'
//  '<S162>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Ideal P Gain Fdbk'
//  '<S163>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Integrator'
//  '<S164>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Integrator ICs'
//  '<S165>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/N Copy'
//  '<S166>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/N Gain'
//  '<S167>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/P Copy'
//  '<S168>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Parallel P Gain'
//  '<S169>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Reset Signal'
//  '<S170>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Saturation'
//  '<S171>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Saturation Fdbk'
//  '<S172>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Sum'
//  '<S173>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Sum Fdbk'
//  '<S174>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tracking Mode'
//  '<S175>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tracking Mode Sum'
//  '<S176>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tsamp - Integral'
//  '<S177>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tsamp - Ngain'
//  '<S178>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/postSat Signal'
//  '<S179>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/preInt Signal'
//  '<S180>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/preSat Signal'
//  '<S181>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S182>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S183>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S184>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/D Gain/Internal Parameters'
//  '<S185>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/External Derivative/Error'
//  '<S186>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Filter/Disc. Forward Euler Filter'
//  '<S187>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Filter ICs/Internal IC - Filter'
//  '<S188>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/I Gain/Internal Parameters'
//  '<S189>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Ideal P Gain/Passthrough'
//  '<S190>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Ideal P Gain Fdbk/Disabled'
//  '<S191>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Integrator/Discrete'
//  '<S192>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Integrator ICs/Internal IC'
//  '<S193>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/N Copy/Disabled'
//  '<S194>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/N Gain/Internal Parameters'
//  '<S195>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/P Copy/Disabled'
//  '<S196>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Parallel P Gain/Internal Parameters'
//  '<S197>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Reset Signal/External Reset'
//  '<S198>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Saturation/Enabled'
//  '<S199>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Saturation Fdbk/Disabled'
//  '<S200>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Sum/Sum_PID'
//  '<S201>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Sum Fdbk/Disabled'
//  '<S202>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tracking Mode/Disabled'
//  '<S203>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tracking Mode Sum/Passthrough'
//  '<S204>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S205>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/Tsamp - Ngain/Passthrough'
//  '<S206>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/postSat Signal/Forward_Path'
//  '<S207>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/preInt Signal/Internal PreInt'
//  '<S208>' : 'matlab_control/Low-Level Controller/cascaded_pid_controller/dRb Controller/preSat Signal/Forward_Path'
//  '<S209>' : 'matlab_control/Low-Level Controller/execute_command/cmdStatusID'
//  '<S210>' : 'matlab_control/Low-Level Controller/execute_command/commandExecuter'
//  '<S211>' : 'matlab_control/Low-Level Controller/execute_command/quatToEul'
//  '<S212>' : 'matlab_control/Subsystem Reference/derivedStateEstimates'
//  '<S213>' : 'matlab_control/Subsystem Reference/discreteTimeQuatPropagation'
//  '<S214>' : 'matlab_control/Subsystem Reference/dvl_reset'
//  '<S215>' : 'matlab_control/Subsystem Reference/eulToQuat'
//  '<S216>' : 'matlab_control/Subsystem Reference/quatToRotm'
//  '<S217>' : 'matlab_control/Subsystem Reference1/Blank Message'
//  '<S218>' : 'matlab_control/Subsystem Reference1/MATLAB Function'
//  '<S219>' : 'matlab_control/Subsystem Reference1/Publish'
//  '<S220>' : 'matlab_control/VR/Enabled Subsystem'
//  '<S221>' : 'matlab_control/dvl_reset_subscriber/Subscribe'
//  '<S222>' : 'matlab_control/dvl_reset_subscriber/Subscribe/Enabled Subsystem'
//  '<S223>' : 'matlab_control/joystick_output/MATLAB Function'
//  '<S224>' : 'matlab_control/joystick_output/Subscribe'
//  '<S225>' : 'matlab_control/joystick_output/Subscribe/Enabled Subsystem'
//  '<S226>' : 'matlab_control/joystick_to_pwm/joystick_inputs_to_body_forces'
//  '<S227>' : 'matlab_control/pwm_heartbeat_publisher/Blank Message1'
//  '<S228>' : 'matlab_control/pwm_heartbeat_publisher/Publish2'
//  '<S229>' : 'matlab_control/pwm_publisher/Publish'

#endif                                 // matlab_control_h_

//
// File trailer for generated code.
//
// [EOF]
//
