#ifndef MANIPULATOR_HPP
#define MANIPULATOR_HPP

#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/u_int8.hpp"
#include "fd_interface.hpp"
#include <mutex>
#include <vector>
#include <string>
#include <chrono>

using namespace rclcpp;

class Arduino_FD : public Path_FD {
protected:
    int open_file() override;
public:
    Arduino_FD(std::string path);
};


class Manipulator : public rclcpp::Node {
public:
    Manipulator(std::unique_ptr<FD_Interface> arduino_fd);
    const int OFFSET_TO_ONE = 48;

private:
    void command_received_subscription_callback(std_msgs::msg::UInt8::UniquePtr command);
    void send_command_to_arduino(char command);
    bool is_valid_command(uint8_t command);
    
    rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr command_received_subscription;
    std::unique_ptr<FD_Interface> arduino_fd;
};

#endif // MANIPULATOR_HPP