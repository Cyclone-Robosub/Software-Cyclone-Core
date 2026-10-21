#ifndef DATA_LOGGER_HPP
#define DATA_LOGGER_HPP

#include <rclcpp/rclcpp.hpp>
#include <custom_interfaces/msg/drr.hpp>
#include <custom_interfaces/msg/vr.hpp>
#include <custom_interfaces/msg/imu.hpp>
#include <custom_interfaces/msg/pwms.hpp>
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
    Data_Logger(std::unique_ptr<FD_Interface> drr_fd,
                std::unique_ptr<FD_Interface> vr_fd,
                std::unique_ptr<FD_Interface> imu_fd,
                std::unique_ptr<FD_Interface> pwm_fd,
                std::unique_ptr<FD_Interface> events_fd);
    static std::string get_current_time();

private:
    std::unique_ptr<FD_Interface> drr_fd;
    std::unique_ptr<FD_Interface> vr_fd;
    std::unique_ptr<FD_Interface> imu_fd;
    std::unique_ptr<FD_Interface> pwm_fd;
    /* For any events (e.g. go switch, manipulator, etc.)*/
    std::unique_ptr<FD_Interface> events_fd;

    rclcpp::Subscription<custom_interfaces::msg::DRR>::SharedPtr drr_subscription;
    rclcpp::Subscription<custom_interfaces::msg::VR>::SharedPtr vr_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Imu>::SharedPtr imu_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_subscription;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr go_signal_subscription;

    void dead_reck_report_received_callback(custom_interfaces::msg::DRR::UniquePtr drr_msg);
    void velocity_report_received_callback(custom_interfaces::msg::VR::UniquePtr vr_msg);
    void imu_data_received_callback(custom_interfaces::msg::Imu::UniquePtr imu_msg);
    void pwm_data_received_callback(custom_interfaces::msg::Pwms::UniquePtr pwm_msg);
    void go_signal_received_callback(std_msgs::msg::Bool::UniquePtr go_signal_msg);
};

#endif // DATA_LOGGER_HPP