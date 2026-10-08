/***************************************************************************************
 *
 * @Copyright 2023, Inertial Sense Inc. <devteam@inertialsense.com>
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************************/

#include "inertial_sense_ros.h"
#define PARAM_YAML_FILE "../../launch/test_config.yaml"
#ifdef ROS2
using namespace rclcpp;
#endif
#ifdef ROS1
using namespace ros;
#endif
int main(int argc, char**argv)
{
    InertialSenseROS* thing;
    init(argc, argv
#ifdef ROS1
                   , "inertial_sense_node"
#endif
                   );
    //auto nh_ = std::make_shared<rclcpp::Node>("nh_");
    if (true)
    {
      //  std::ifstream paramYamlPath.open(PARAM_YAML_FILE);
        std::string paramsomethingYamlString = "topic: \"inertialsense\"\nport: [/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_205D36773430-if00]\nbaudrate: 921600\nenable_log: false\npublishTf: true                                 # Publish Transform Frame (TR)\nframe_id: \"\"                                    # FIXME: What is this?  is it just the FrameID to use in the ROS messages?\nmag_declination: 0.0\nref_lla: [0.0, 0.0, 0.0]\n# factory_reset: true                             # Reset flash config to factory defaults on startup.\n\n# Hardware platform specifying the IMX carrier board type (i.e. EVB-2) and configuration bits (see ePlatformConfig).  The platform type is used to simplify the GPS and I/O configuration process.\n# platform: 10                                    # PLATFORM_CFG_TYPE_RUG3_G2\n# platform: 11                                    # PLATFORM_CFG_TYPE_EVB2_G2\n# platform: 15                                    # PLATFORM_CFG_TYPE_IG1_G2\n\n# Overwrite IMX ioConfig (see eIoConfig)\n# Set DID_FLASH_CONFIG.ioConfig: 0x025c2060\n# Set DID_FLASH_CONFIG.platformConfig: 0x0000000c\n# Set DID_FLASH_CONFIG.RTKCfgBits: 0x00000400\n# ioConfig: 0x026ca060      # EVB2: GPS1 Ser1 F9P, GPS2 Ser2 F9P, PPS G8\n# ioConfig: 0x025CA060      # EVB2: GPS1 Ser1 F9P, GPS2 Ser0 F9P, PPS G8\n# ioConfig: 0x0244a060      # EVB2: GPS1 Ser1 F9P, GPS2 disabled F9P, PPS G8\n\n# Service Points - should we allow these?  Or should they always be hardcoded?\n# service_endpoints:\n  # REF_LLA: \"set_refLLA\"                         # /value /current\n  # MAG_CAL: \"mag_cal\"                            # /single /multi\n  # FIRMWARE: \"firmware\"                          # /get /update\n\n# Publish the firmware upgrade progress/status\nfirmware_status:\n  message:\n    topic: \"firmware_status\"\n    enable: true\n\ntransform_frame:\n  # static_transform:\n  message:\n    topic: \"\"\n    enable: true\n\nins:\n  rotation: [0, 0, 0]                           # Rotation in radians about the X,Y,Z axes from Sensor Frame to Intermediate Output Frame.  Order applied: Z,Y,X.\n  offset: [0, 0 ,0]                             # X,Y,Z offset in meters from Intermediate Output Frame to INS Output Frame.\n  navigation_dt_ms: 16                          # EKF update period.  IMX-5:  16 default, 8 max.  Use `msg/ins.../period` to reduce INS output data rate.\n  # navigation_dt_ms: 4                           # EKF update period.  uINS-3: 4  default, 1 max.  Use `msg/ins.../period` to reduce INS output data rate.\n  dynamic_model: 4                              # FIXME: these should be named types/srtring (PORTABLE, STATIONARY, AGV, UAV, etc)\n  enable_covariance: false                      # Include covariance data in odom_ins_... messages\n  messages:\n    did_ins1:\n      topic: \"ins_eul_uvw_ned\"\n      enable: true\n\nsensors:\n  messages:  \n    imu:              # Publish IMU angular rates and linear acceleration\n      topic: \"imu\"\n      enable: true\n      period: 1\n    pimu:             # Publish preintegrated IMU delta theta and delta velocity\n      topic: \"pimu\"\n      enable: true\n      period: 1\n    magnetometer:\n      topic: \"mag\"\n      enable: true\n      period: 1";
        //std::cout << "\n\nLoading YAML paramfile: " << paramYamlPath << "\n\n";
        YAML::Node node;
        try
        {
            node = YAML::Load(paramsomethingYamlString);
        }
        catch (const YAML::BadFile &bf)
        {
            std::cout << "Loading file \"" << paramsomethingYamlString << "\" failed.  Using default parameters.\n\n";
            node = YAML::Node(YAML::NodeType::Undefined);
        }

        thing = new InertialSenseROS(node);
    }
    else
    {
        thing = new InertialSenseROS;
    }

    thing->initialize();
    while (ok())
    {
#ifdef ROS2
        spin_some(thing->nh_);
#endif
#ifdef ROS1
        ros::spinOnce();
#endif
        thing->update();
    }
    return 0;
}
