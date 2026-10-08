#pragma once


#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "custom_interfaces/msg/pwms.hpp"
#include "custom_interfaces/srv/control_mode.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "std_srvs/srv/set_bool.hpp"

enum ControlMode {Disabled=0, CLI=1, CTRL=2, Echo=3};


class SoftMux : public rclcpp::Node {
    public:
        friend class TestSoftMuxInterface_MuxSendCtrlPwms_Test;
        friend class TestSoftMuxInterface_MuxSendCliPwms_Test;
        friend class TestSoftMuxInterface_MuxTestSetModeService_Test;
        friend class TestSoftMuxInterface_MuxTestGetModeService_Test;
        friend class TestSoftMuxInterface_MuxTestInputHB_Test;
        friend class TestSoftMuxInterface_NoCtrlHeartbeatTriggersStop_Test;
        friend class TestSoftMuxInterface_NoCliHeartbeatTriggersStop_Test;
        friend class TestSoftMuxInterface_NoCtrlHeartbeatonCli_Test;
        friend class TestSoftMuxInterface_NoCliHeartbeatonCtrl_Test;
        friend class TestSoftMuxInterface_MuxHeartbeat_Test;
        friend class TestSoftMuxInterface_MuxTestSetModeEcho_Test;
        friend class TestSoftMuxInterface_NoEchoHeartbeatTriggersStop_Test;
        friend class TestSoftMuxInterface_DisabledPublishesStopSets_Test;
        SoftMux();
        void pwm_ctrl_callback(custom_interfaces::msg::Pwms::UniquePtr pwm);
        void pwm_cli_callback(custom_interfaces::msg::Pwms::UniquePtr pwm);
        void pwm_echo_callback(custom_interfaces::msg::Pwms::UniquePtr pwm);
        void set_mode_srv(const std::shared_ptr<custom_interfaces::srv::ControlMode::Request> request);
        void pub_mode_srv(const std::shared_ptr<std_srvs::srv::SetBool::Request> request, const std::shared_ptr<std_srvs::srv::SetBool::Response> response);
    private:
        void heartbeat_callback();
        void ctrl_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
        void ctrl_heartbeat_check();
        void cli_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
        void cli_heartbeat_check();
        void echo_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
        void echo_heartbeat_check();
        void pwm_cmd_publish(custom_interfaces::msg::Pwms::UniquePtr pwm);
        void publish_stop_command();
        void mux_heartbeat_send();
        void publish_current_control_mode();
        void publish_stop_if_disabled();
       
        rclcpp::Service<custom_interfaces::srv::ControlMode>::SharedPtr control_mode_service;
        rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr force_pub_service;
       
        rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_ctrl_subscriber;
        rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_cli_subscriber;
        rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_echo_subscriber;
        rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr ctrl_heartbeat_subscriber;
        rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr cli_heartbeat_subscriber;
        rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr echo_heartbeat_subscriber;
       
        rclcpp::Publisher<custom_interfaces::msg::Pwms>::SharedPtr pwm_cmd_publisher;
        rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr current_control_mode_publisher;
        rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr mux_heartbeat_publisher;
       
        rclcpp::TimerBase::SharedPtr heartbeat_timer;

        std::chrono::time_point<std::chrono::steady_clock> recent_ctrl_heartbeat;
        std::chrono::time_point<std::chrono::steady_clock> recent_cli_heartbeat;
        std::chrono::time_point<std::chrono::steady_clock> recent_echo_heartbeat;


        int control_mode;
        bool no_ctrl_heartbeat;
        bool no_cli_heartbeat;
        bool no_echo_heartbeat;
};
