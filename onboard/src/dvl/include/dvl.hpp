#pragma once

#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include <string>
#include <cstdint>
#include <array>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include "custom_interfaces/msg/config.hpp"
#include "custom_interfaces/msg/vr.hpp"
#include "custom_interfaces/msg/drr.hpp"
#include "custom_interfaces/srv/set_config.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "std_msgs/msg/empty.hpp"
#include "fd_interface.hpp"

class DVL_FD : public Path_FD {
public:
    DVL_FD(std::string path);
protected:
    int open_file() override;
};

namespace dvl {
struct velocity_report {
    float vx = 0, vy = 0, vz = 0, altitude = 0, fom = 0, time = 0;
    std::array<float,9> covariance = {0}; // all zeroes
    int64_t time_of_validity = 0, time_of_transmission = 0;
    char valid = 'n';
    uint8_t status = 0x00; 
};

struct dead_reck_report {
    int64_t time_stamp = 0;
    float x = 0, y = 0, z = 0, pos_std = 0, roll = 0, pitch = 0, yaw = 0;
    uint8_t status = 0x00;
};

struct config_report {
    float speed_of_sound;
    float mounting_rotation_offset;
    std::string acoustic_enabled; //y or n
    std::string dark_mode_enabled; //y or n
    std::string range_mode = "auto";
    std::string periodic_cycling_enabled; //y or n
};

// Protocol definitions
static constexpr uint8_t SOP = 'w'; //start of package
static constexpr std::array<uint8_t,2> EOP = {'\n','\r'}; //possible end of packets bytes
static constexpr uint8_t DIR_CMD = 'c'; //command
static constexpr uint8_t DIR_RESP = 'r'; //response
static constexpr uint8_t CS = '*'; //checksum 

//valid options for [CMD] in outgoing message
static constexpr uint8_t CMD_SET_SETTINGS = 's';
static constexpr uint8_t CMD_GET_SETTINGS = 'c';
static constexpr uint8_t CMD_RESET_DR = 'r';
static constexpr uint8_t CMD_TRIGGER_PING = 'x';
static constexpr uint8_t CMD_CALIBRATE_GYRO = 'g';

static constexpr uint8_t REC_VR = 'z'; //receive velocity report
static constexpr uint8_t REC_TR = 'u'; //receive transducer report
static constexpr uint8_t REC_DRR = 'p'; //receive dead reckoning report

static constexpr uint8_t ACK = 'a';

// list of valid outgoing commands
static constexpr std::array<char,9> VALID_OUT = {CMD_SET_SETTINGS, CMD_GET_SETTINGS, CMD_RESET_DR, CMD_TRIGGER_PING, CMD_CALIBRATE_GYRO};

// list of valid received commands
static constexpr std::array<char,12> VALID_IN = {REC_VR, REC_TR, REC_DRR, CMD_SET_SETTINGS, CMD_GET_SETTINGS, CMD_RESET_DR, CMD_TRIGGER_PING, CMD_CALIBRATE_GYRO};

uint8_t crc8(uint8_t*, int);

class DVL : public rclcpp::Node {
    public:
        friend class TestDVLInterface_BadVelocityReport_Test;
        // PUBLIC API //
        DVL(std::unique_ptr<FD_Interface> dvl_fd);

        //reads from actual DVL
        velocity_report readVelocityReport(); //velocity report
        dead_reck_report readDRReport(); //dead reckoning report
        config_report readConfig();

        void publishVR();
        void publishDRR();
        void publishConfig();

        //sets (returns true if acknowledge was received)
        void setConfig(const std::shared_ptr<custom_interfaces::srv::SetConfig::Request> request, const std::shared_ptr<custom_interfaces::srv::SetConfig::Response> response);
        void resetDRR(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response); //reset the dead reckoning report
        void resetGyro(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response); //zero the gyroscope
        void triggerPing(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response);

        char getCommandFromSerial();
        char getCommandFromSerial(std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::milliseconds TIMEOUT);
        void publishCommandFromSerial(char cmd);
        void workLoop();
    private:
        rclcpp::Publisher<custom_interfaces::msg::VR>::SharedPtr velocity_report_publisher;
        rclcpp::Publisher<custom_interfaces::msg::DRR>::SharedPtr drr_report_publisher;
        rclcpp::Publisher<custom_interfaces::msg::Config>::SharedPtr config_publisher;
        rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr heartbeat_publisher;

        rclcpp::Service<custom_interfaces::srv::SetConfig>::SharedPtr config_service;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr drr_service;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr gyro_service;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr vr_service;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr trigger_ping;

        /*
        Outgoing messages are expected in the format "[SOP][DIR_CMD][CMD],[option 1],[option 2],...,[option n],[CS],[CHECKSUM]\n". Options are only needed for some commands

        Incoming messages are expected in the format "[SOP]{DIR_RESP][CMD],[option 1],[option 2],...,[option n],[CS],[CHECKSUM]\n" Options are only needed for some commands. EOP may be "\n", "\r", or "\r\n"
        */

        std::mutex dvl_mutex;

        // PRIVATE VARS //
        std::unique_ptr<FD_Interface> fd;
        velocity_report vr, error_vr;
        dead_reck_report drr, error_drr;

        config_report error_config, config;
        
        std::string version; //Protocol version "major.minor.patch"
        std::string product_details;

        rclcpp::TimerBase::SharedPtr heartbeat_timer;

        // PRIVATE METHODS //
        bool getResponse(const char expected_response);
        bool parseResponse(std::string& complete_line); //parses text string from DVL into the results structure
        bool sendCommand(uint8_t cmd, const std::vector<std::string>& options = {});
        void publish_heartbeat_callback();
        
};

}
