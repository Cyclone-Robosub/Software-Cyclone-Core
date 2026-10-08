#ifndef DATA_LOGGER_HPP
#define DATA_LOGGER_HPP

#include <rclcpp/rclcpp.hpp>
#include <custom_interfaces/msg/drr.hpp>
#include <custom_interfaces/msg/vr.hpp>
#include "std_msgs/msg/bool.hpp"
#include "fd_interface.hpp"
#include <mutex>
#include <string>
#include <vector>
#include <ctime>

using namespace rclcpp;

class Data_FD : public Path_FD {
protected:
    int open_file() override;
public:
    Data_FD(std::string path);
};

class Data_Logger : public rclcpp::Node {
public:
    Data_Logger(std::unique_ptr<FD_Interface> drr_fd, std::unique_ptr<FD_Interface> vr_fd);
    static std::string get_current_time();

private:
    std::unique_ptr<FD_Interface> drr_fd;
    std::unique_ptr<FD_Interface> vr_fd;
    // std::unique_ptr<FD_Interface> imu_fd;

    rclcpp::Subscription<custom_interfaces::msg::DRR>::SharedPtr drr_subscription;
    rclcpp::Subscription<custom_interfaces::msg::VR>::SharedPtr vr_subscription;

    void dead_reck_report_received_callback(custom_interfaces::msg::DRR::UniquePtr drr_msg);
    void velocity_report_received_callback(custom_interfaces::msg::VR::UniquePtr vr_msg);
};

#endif // DATA_LOGGER_HPP