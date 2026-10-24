#ifndef SIMPLE_BMS_HPP
#define SIMPLE_BMS_HPP

#include <custom_interfaces/msg/battery.hpp>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include "fd_interface.hpp"

using namespace rclcpp;

class ADC_FD : public Path_FD {
protected:
    int open_file() override;
    bool config();
public:
    ADC_FD(std::string path);
};

class SimpleBMS : public rclcpp::Node {

    public:
        struct BatteryStatus {
            float voltage;
            float current;
        };

        SimpleBMS(std::unique_ptr<FD_Interface> pico_fd);
        ~SimpleBMS();
       
        struct ConfigValues {
            uint8_t conf_reg; // Config register
            uint8_t lsb; // See pg 25-26 of the ads1115.pdf (or below) for what the individual bits mean
            uint8_t msb;
            uint8_t conv_reg_val;
        };
        bool config_i2c(SimpleBMS::ConfigValues conf);

    private:
        const ConfigValues VOLTAGE_CONFIG = { 
            // AIN_P and AIN_N denote the selected positive and negative inputs.
            /* Value of 8142 -- 1000001101000010
            1   - Start a single conversion
            000 - Input multiplexer configuration - AIN_P = AIN0 and AIN_N = AIN1
            000 - FSR=+- 6.144V
            1   - Operate in single-shot mode
            010 - Data rate of 32SPS
            0   - Comparator operating mode: Traditional
            0   - Alert/ready pin polarity: Active low
            0   - Assert pin does not latch
            10  - Assert the alert/ready pin after four consecutive conversions which exceed the limit
            */
            0x01,
            0x42,
            0x81,
            0x00
        };
        const ConfigValues CURRENT_CONFIG = {
            // Only change from the values in the voltage config is bit 13 such that the multiplexer config is 010, meaning AIN_P = AIN1 and AIN_N = AIN3
            0x01,
            0x42,
            0xA1,
            0x00
        };
        int i2c_fd = -1;
        
        std::unique_ptr<FD_Interface> bms_fd;
        const char* I2C_FD = "/dev/i2c-1";
        const int BMS_ADDRESS = 0x48;

        rclcpp::TimerBase::SharedPtr battery_timer;
        rclcpp::Publisher<custom_interfaces::msg::Battery>::SharedPtr bms_publisher;

        bool init_i2c();
        void bms_callback();
        float readBMS(bool isVoltage);
        void publish_bms(BatteryStatus status);
};

#endif
