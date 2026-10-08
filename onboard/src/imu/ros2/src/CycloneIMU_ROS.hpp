
#include <iostream>

#include "../shared/include/inertial_sense_ros.h"
#include "custom_interfaces/msg/imu.hpp"
#include "rclcpp/rclcpp/publisher.hpp"
#include "rclcpp/rclcpp/rclcpp.hpp"
#include "rclcpp/rclcpp/time.hpp"
#include "inertial_sense_ros2/msg/didins1.hpp"
#include "rclcpp/rclcpp/timer.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "sensor_msgs/msg/fluid_pressure.hpp"
#include "std_msgs/std_msgs/msg/string.hpp"
#include <atomic>
#include <memory>
std::string params = "topic: \"inertialsense\"\nport: [/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_205D36773430-if00]\nbaudrate: 921600\nenable_log: false\npublishTf: true                                 # Publish Transform Frame (TR)\nframe_id: \"\"                                    # FIXME: What is this?  is it just the FrameID to use in the ROS messages?\nmag_declination: 0.0\nref_lla: [0.0, 0.0, 0.0]\n# factory_reset: true                             # Reset flash config to factory defaults on startup.\n\n# Hardware platform specifying the IMX carrier board type (i.e. EVB-2) and configuration bits (see ePlatformConfig).  The platform type is used to simplify the GPS and I/O configuration process.\n# platform: 10                                    # PLATFORM_CFG_TYPE_RUG3_G2\n# platform: 11                                    # PLATFORM_CFG_TYPE_EVB2_G2\n# platform: 15                                    # PLATFORM_CFG_TYPE_IG1_G2\n\n# Overwrite IMX ioConfig (see eIoConfig)\n# Set DID_FLASH_CONFIG.ioConfig: 0x025c2060\n# Set DID_FLASH_CONFIG.platformConfig: 0x0000000c\n# Set DID_FLASH_CONFIG.RTKCfgBits: 0x00000400\n# ioConfig: 0x026ca060      # EVB2: GPS1 Ser1 F9P, GPS2 Ser2 F9P, PPS G8\n# ioConfig: 0x025CA060      # EVB2: GPS1 Ser1 F9P, GPS2 Ser0 F9P, PPS G8\n# ioConfig: 0x0244a060      # EVB2: GPS1 Ser1 F9P, GPS2 disabled F9P, PPS G8\n\n# Service Points - should we allow these?  Or should they always be hardcoded?\n# service_endpoints:\n  # REF_LLA: \"set_refLLA\"                         # /value /current\n  # MAG_CAL: \"mag_cal\"                            # /single /multi\n  # FIRMWARE: \"firmware\"                          # /get /update\n\n# Publish the firmware upgrade progress/status\nfirmware_status:\n  message:\n    topic: \"firmware_status\"\n    enable: true\n\ntransform_frame:\n  # static_transform:\n  message:\n    topic: \"\"\n    enable: true\n\nins:\n  rotation: [0, 0, 0]                           # Rotation in radians about the X,Y,Z axes from Sensor Frame to Intermediate Output Frame.  Order applied: Z,Y,X.\n  offset: [0, 0 ,0]                             # X,Y,Z offset in meters from Intermediate Output Frame to INS Output Frame.\n  navigation_dt_ms: 16                          # EKF update period.  IMX-5:  16 default, 8 max.  Use `msg/ins.../period` to reduce INS output data rate.\n  # navigation_dt_ms: 4                           # EKF update period.  uINS-3: 4  default, 1 max.  Use `msg/ins.../period` to reduce INS output data rate.\n  dynamic_model: 4                              # FIXME: these should be named types/srtring (PORTABLE, STATIONARY, AGV, UAV, etc)\n  enable_covariance: true                      # Include covariance data in odom_ins_... messages\n  messages:\n    did_ins1:\n      topic: \"ins_eul_uvw_ned\"\n      enable: true\n\nsensors:\n  messages:  \n    imu:              # Publish IMU angular rates and linear acceleration\n      topic: \"imu\"\n      enable: true\n      period: 1\n    pimu:             # Publish preintegrated IMU delta theta and delta velocity\n      topic: \"pimu\"\n      enable: true\n      period: 1\n    magnetometer:\n      topic: \"mag\"\n      enable: true\n      period: 1";
YAML::Node nodesomething;
void SetupInertialSenseROS() {
    try {
        nodesomething = YAML::Load(params);
    } catch (const YAML::BadFile& bf) {
        //    std::cout << "Loading file \"" << paramYamlPath << "\" failed.  Using default parameters.\n\n";
        nodesomething = YAML::Node(YAML::NodeType::Undefined);
    }
    InertialSenseROS IMUROS = InertialSenseROS(nodesomething);
    IMUROS.initialize();
    while(true){
    rclcpp::spin_some(IMUROS.nh_);
    IMUROS.update();
    }
}
class CycloneIMU_ROS : public rclcpp::Node {
   public:
    CycloneIMU_ROS() : Node("IMU_ROS_helper_node") {
        callbackSensors = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        auto commandOptions = rclcpp::SubscriptionOptions();
        commandOptions.callback_group = callbackSensors;

        imu_subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "imu", 10,
            std::bind(&CycloneIMU_ROS::imu_callback, this, std::placeholders::_1),
            commandOptions);
        DIDINS1_sub = this->create_subscription<inertial_sense_ros2::msg::DIDINS1>(
    "ins_eul_uvw_ned", 10, std::bind(&CycloneIMU_ROS::DIDINS1_callback, this, std::placeholders::_1), commandOptions);

        mag_subscription_ = this->create_subscription<sensor_msgs::msg::MagneticField>(
            "mag", 10,
            std::bind(&CycloneIMU_ROS::mag_callback, this, std::placeholders::_1),
            commandOptions);

        odom_sub = this->create_subscription<nav_msgs::msg::Odometry>(
            "odom_ins_ned", 10,
            std::bind(&CycloneIMU_ROS::odom_callback, this, std::placeholders::_1),
            commandOptions);

        pressure_sub = this->create_subscription<sensor_msgs::msg::FluidPressure>(
            "baro", 10,
            std::bind(&CycloneIMU_ROS::pressure_callback, this, std::placeholders::_1),
            commandOptions);

        imu_publisher = this->create_publisher<custom_interfaces::msg::Imu>("imu_custom", 10);

        inertialSenseROS = std::thread([this]() { SetupInertialSenseROS(); });
        PublisherThread = std::thread([this]() { Controls_Publisher(); });
    }
   private:
    void imu_callback(std::shared_ptr<sensor_msgs::msg::Imu> msg);
    void mag_callback(std::shared_ptr<sensor_msgs::msg::MagneticField> msg);
    void odom_callback(std::shared_ptr<nav_msgs::msg::Odometry> msg);
    void pressure_callback(std::shared_ptr<sensor_msgs::msg::FluidPressure> msg);
    void DIDINS1_callback(const inertial_sense_ros2::msg::DIDINS1::SharedPtr msg);
    void Controls_Publisher();

    rclcpp::Subscription<inertial_sense_ros2::msg::DIDINS1>::SharedPtr DIDINS1_sub;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
    rclcpp::Subscription<sensor_msgs::msg::MagneticField>::SharedPtr mag_subscription_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_subscription_;
    rclcpp::Publisher<custom_interfaces::msg::Imu>::SharedPtr imu_publisher;
    rclcpp::Subscription<sensor_msgs::msg::FluidPressure>::SharedPtr pressure_sub;
    
    rclcpp::CallbackGroup::SharedPtr callbackSensors;

    std::thread inertialSenseROS;
    std::thread PublisherThread;
    std::shared_ptr<sensor_msgs::msg::Imu> imu_ptr;
    std::shared_ptr<nav_msgs::msg::Odometry> odom_ptr;
    std::shared_ptr<sensor_msgs::msg::MagneticField> mag_ptr;
    std::shared_ptr<sensor_msgs::msg::FluidPressure> pressure_ptr;
    std::mutex imu_mutex;
    std::mutex odom_mutex;
    std::mutex mag_mutex;
    std::mutex DIDINS1_mutex;
    std::mutex pressure_mutex;
    inertial_sense_ros2::msg::DIDINS1::SharedPtr DIDINS1_ptr;
    custom_interfaces::msg::Imu custom_msg = custom_interfaces::msg::Imu();
    std::atomic<int> odomCount;
    double angular_velocity_x = 0.0;
    double angular_velocity_y = 0.0;
    double angular_velocity_z = 0.0;
    double linear_acceleration_x = 0.0;
    double linear_acceleration_y = 0.0;
    double linear_acceleration_z = 0.0;
    float somerandomValue = 0.0;
    double mag_field_x = 0.0;
    double mag_field_y = 0.0;
    double mag_field_z = 0.0;
    double pressure = 0.0;
};
