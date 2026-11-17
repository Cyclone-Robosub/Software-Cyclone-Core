#pragma once

#include "custom_interfaces/msg/loginfo.hpp"
#include <string>
#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>

class log_publisher
{
public:
    // name is the log file the central logger writes to
    log_publisher(rclcpp::Node& node, const std::string& name);

    // contents longer than the Loginfo contents (256 bytes) field are truncated
    void log(const std::string& contents);

private:
    rclcpp::Publisher<custom_interfaces::msg::Loginfo>::SharedPtr publisher;
    std::string name;
};
