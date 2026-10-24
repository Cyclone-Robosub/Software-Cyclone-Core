#ifndef MISSION_MANAGER_HPP
#define MISSION_MANAGER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "custom_interfaces/msg/command_result.hpp"
#include "custom_interfaces/msg/mission_status.hpp"
#include "custom_interfaces/srv/get_file.hpp"
#include "robot_command.hpp"
#include "mission_file_parser.hpp"
#include <mutex>
#include <exception>

class MissionManager : public rclcpp::Node {
public:
    MissionManager(std::unique_ptr<MissionFileParser> parser);
    void work_loop();

private:
    /* Runs in a seperate thread so that mission triggering can happen independently */
    void run_mission();
    void pub_ready_status();

    void trigger_ready_signal(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void pub_ready_status_from_service(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void go_signal_callback(std_msgs::msg::Bool::SharedPtr signal);
    void add_mission_file_callback(const std::shared_ptr<custom_interfaces::srv::GetFile::Request> request, std::shared_ptr<custom_interfaces::srv::GetFile::Response> response);
    void add_mission_list_callback(const std::shared_ptr<custom_interfaces::srv::GetFile::Request> request, std::shared_ptr<custom_interfaces::srv::GetFile::Response> response);
    void command_result_callback(custom_interfaces::msg::CommandResult::SharedPtr result);
    void heartbeat_callback();
    void publish_mission_status_throttled(std::shared_ptr<Command> current_command, std::string mission_name);
    void publish_mission_status_unthrottled(std::shared_ptr<Command> current_command, std::string mission_name);
    std::array<uint8_t,32> string_to_array(std::string string);

    /* Sent to MATLAB */
    rclcpp::Publisher<custom_interfaces::msg::Command>::SharedPtr command_publisher;
    /* MissionManager's own heartbeat for viewing in TUI */
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr heartbeat_publisher;
    /* Whether MissionManager is ready or not. Publishes on change, or when requested. */
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr current_ready_status_publisher;
    /* Information about the state of the current mission and command, to be used for display in dashboard */
    rclcpp::Publisher<custom_interfaces::msg::MissionStatus>::SharedPtr current_mission_status_publisher;

    /* Go signal from MicroROS. Receives a bool for backwards compatiblity, but value is ignored. */
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr go_signal_subscription;
    /* Result from MATLAB. Either success or failure. Technically supports "RUNNING", but this is ignored. */
    rclcpp::Subscription<custom_interfaces::msg::CommandResult>::SharedPtr command_result_subscription;

    /* On receive (from TUI), toggle ready status */
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr toggle_ready_service;
    /* On receive, force a publish of the current ready/unready state */
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr ready_pub_service;

    /*Add given .json file to parsed mission files*/
    rclcpp::Service<custom_interfaces::srv::GetFile>::SharedPtr add_mission_file;
    /*Add all .json files in given .txt file to parsed mission files*/
    rclcpp::Service<custom_interfaces::srv::GetFile>::SharedPtr add_mission_list;

    rclcpp::TimerBase::SharedPtr heartbeat_timer;

    std::unique_ptr<MissionFileParser> parser;

    std::mutex initiate_mission_mutex;
    std::mutex status_mutex;

    volatile bool status_received;
    volatile bool command_successful;
    volatile bool ready;
    volatile bool go;
    volatile bool mission_running;
    volatile bool cancel_mission;
    volatile bool timed_out;
    std::chrono::time_point<std::chrono::steady_clock> last_mission_status_update;
    std::chrono::time_point<std::chrono::steady_clock> last_go_switch_update;
};

#endif // MISSION_MANAGER_HPP
