#pragma once

#include "custom_interfaces/msg/loginfo.hpp"
#include <array>
#include <cstdint>
#include <filesystem>
#include <rclcpp/node.hpp>

class central_logger : public rclcpp::Node
{
public:
    central_logger();

private:
    rclcpp::Subscription<custom_interfaces::msg::Loginfo>::SharedPtr loginfo_subscriber;

    void central_logger_callback(custom_interfaces::msg::Loginfo::UniquePtr msg);
    void log_to_file(const std::array<std::uint8_t, 32>& name, const std::array<std::uint8_t, 256>& contents);

    std::filesystem::path current_run_dir;
};
