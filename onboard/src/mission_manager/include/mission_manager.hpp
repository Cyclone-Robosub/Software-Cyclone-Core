#ifndef MISSION_MANAGER_HPP
#define MISSION_MANAGER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "robot_command.hpp"
#include "mission_file_parser.hpp"

class MissionManager : public rclcpp::Node {
public:
    MissionManager(std::unique_ptr<MissionFileParser> parser);

private:
    void run_missions();
    void pub_ready_status();

    void trigger_ready_signal(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void pub_ready_status_from_service(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void go_signal_callback(std_msgs::msg::Bool::SharedPtr signal);
    void heartbeat_callback();

    /* Sent to MATLAB */
    rclcpp::Publisher<custom_interfaces::msg::Command>::SharedPtr command_publisher;
    /* MissionManager's own heartbeat for viewing in TUI */
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr heartbeat_publisher;
    /* Whether MissionManager is ready or not. Publishes on change, or when requested. */
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr current_ready_status_publisher;

    /* Go signal from MicroROS. Receives a bool for backwards compatiblity, but value is ignored. */
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr go_signal_subscription;

    /* On receive (from TUI), toggle ready status */
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr toggle_ready_service;
    /* On receive, force a publish of the current ready/unready state */
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr ready_pub_service;

    rclcpp::TimerBase::SharedPtr heartbeat_timer;

    std::unique_ptr<MissionFileParser> parser;
    std::vector<std::shared_ptr<Root>> missions;

    volatile bool status_received;
    bool command_successful;
    volatile bool ready;
    bool missions_running;
};

#endif // MISSION_MANAGER_HPP
