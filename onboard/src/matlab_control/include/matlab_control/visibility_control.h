#ifndef MATLAB_CONTROL__VISIBILITY_CONTROL_H_
#define MATLAB_CONTROL__VISIBILITY_CONTROL_H_
#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define MATLAB_CONTROL_EXPORT __attribute__ ((dllexport))
    #define MATLAB_CONTROL_IMPORT __attribute__ ((dllimport))
  #else
    #define MATLAB_CONTROL_EXPORT __declspec(dllexport)
    #define MATLAB_CONTROL_IMPORT __declspec(dllimport)
  #endif
  #ifdef MATLAB_CONTROL_BUILDING_LIBRARY
    #define MATLAB_CONTROL_PUBLIC MATLAB_CONTROL_EXPORT
  #else
    #define MATLAB_CONTROL_PUBLIC MATLAB_CONTROL_IMPORT
  #endif
  #define MATLAB_CONTROL_PUBLIC_TYPE MATLAB_CONTROL_PUBLIC
  #define MATLAB_CONTROL_LOCAL
#else
  #define MATLAB_CONTROL_EXPORT __attribute__ ((visibility("default")))
  #define MATLAB_CONTROL_IMPORT
  #if __GNUC__ >= 4
    #define MATLAB_CONTROL_PUBLIC __attribute__ ((visibility("default")))
    #define MATLAB_CONTROL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define MATLAB_CONTROL_PUBLIC
    #define MATLAB_CONTROL_LOCAL
  #endif
  #define MATLAB_CONTROL_PUBLIC_TYPE
#endif
#endif  // MATLAB_CONTROL__VISIBILITY_CONTROL_H_
// Generated 13-Jul-2026 00:21:16
 