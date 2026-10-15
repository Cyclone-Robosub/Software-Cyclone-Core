#pragma once

#include "custom_interfaces/msg/loginfo.hpp"
#include "rclcpp/rclcpp.hpp"

#include <chrono>

class central_logger : public rclcpp::NODE{
public:
    central_logger();

    void central_logger_callback(custom_interface::msg::Loginfo msg);

    void check_name();

private:
    rclcpp:Subscription<custom_interfaces::msg::Loginfo>::SharedPtr loginfo_subscriber;
    char[32] current_name;
    char[256] current_contents;

}