#ifndef TUI_HPP
#define TUI_HPP

#include <chrono>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm> // for std::min()
#include "rclcpp/rclcpp.hpp"
#include "tui_interface.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "std_srvs/srv/set_bool.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "custom_interfaces/msg/pwms.hpp"
#include "custom_interfaces/msg/vr.hpp"
#include "custom_interfaces/msg/drr.hpp"
#include "custom_interfaces/msg/debug.hpp"
#include "custom_interfaces/srv/control_mode.hpp"

using namespace rclcpp;

typedef struct pwm_data {
    std::array<int,8> pwms = {0};
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
} PWM_Data;

typedef struct dvl_data {
    double x = 0;
    double y = 0;
    double z = 0;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
} DVL_Data;

typedef struct debug_message {
    char message[257] = {0}; // one byte more so that we get a guaranteed null termination
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
} Debug_Message;

class Dashboard_TUI : public TUI_Interface {
    public:
        explicit Dashboard_TUI() {};
        virtual void display_tui(va_list args) override;
        virtual void process_input() override;
        virtual void refresh_display(int numArgs, ...) override;
    private:
        void jump_to_column(const int col_number);
        void reset_cursor_pos();
        void write_header(std::string header, const int col_number);
        void write_selectable_header(std::string header, int current_mode, int active_mode, const int col_number);
        void display_status_with_colour(bool heartbeat, const int col_number, std::string colour_string);
        void display_critical_status(bool heartbeat, const int col_number);
        void display_warning_status(bool heartbeat, const int col_number);
        void display_noncritical_status(bool heartbeat, const int col_number);
        void display_escalatable_critical_status(int current_mode, int critical_mode, bool heartbeat, const int col_number);
        void display_escalatable_warning_status(int current_mode, int warning_mode, bool heartbeat, const int col_number);
        void display_mission_manager(int current_mux_mode, int warning_mode, bool heartbeat, bool ready, const int col_number);
        void display_ready(const int col_number);
        void display_pwms(std::array<int,8> pwms, const int col_number, bool fresh);
        void fill_right_col(const int col_number);
        void display_all_pwms(PWM_Data pwms_cmd, PWM_Data pwms_cli, PWM_Data pwms_ctrl, PWM_Data pwms_echo);
        void display_connection_info(bool connection_ok, double seconds_since_ping, double ping_rtt, const int col_number);
        void printDVLCoordinate(double coordinate);
        void display_vr(const int col_number, DVL_Data velocity);
        void display_drr(const int col_number, DVL_Data position, DVL_Data orientation);
        void display_commands();
        void display_debug(const int col_number, Debug_Message debug);
        std::string dvl_fresh(DVL_Data dvl_data);
        std::string pwm_fresh(PWM_Data pwm_data);
        std::chrono::time_point<std::chrono::steady_clock> fresh_evaluation_time;
        const int col_1 = 0;
        const int col_2 = 35;
        const int col_2_1 = 45;
        const int col_2_2 = 55;
        const int col_2_3 = 66;
        const int col_2_4 = 77;
        const int col_3 = 88;
        int prev_debug_length = 256; // don't like this, but need a way to clean up old text...
};

class Dashboard : public rclcpp::Node {
public:
    Dashboard(std::unique_ptr<TUI_Interface> tui);
    void work_loop();
    void ping_loop();
private:
    std::unique_ptr<TUI_Interface> tui;

    // Heartbeats
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr thrust_interface_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr mux_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr cli_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr ctrl_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr echo_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr dvl_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr joystick_heartbeat_subscription;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr mission_manager_heartbeat_subscription;
    
    // Data
    rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_cmd_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_cli_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_ctrl_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_echo_subscription;
    rclcpp::Subscription<custom_interfaces::msg::VR>::SharedPtr dvl_vr_subscription;
    rclcpp::Subscription<custom_interfaces::msg::DRR>::SharedPtr dvl_drr_subscription;
    rclcpp::Subscription<custom_interfaces::msg::Debug>::SharedPtr matlab_debug_subscription;
    rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr current_control_mode_subscription;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr ready_status_subscription;
    
    void thrust_interface_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void mux_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void cli_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void ctrl_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void echo_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void dvl_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void joystick_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);
    void mission_manager_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat);

    void pwm_cmd_callback(custom_interfaces::msg::Pwms::UniquePtr pwms);
    void pwm_cli_callback(custom_interfaces::msg::Pwms::UniquePtr pwms);
    void pwm_ctrl_callback(custom_interfaces::msg::Pwms::UniquePtr pwms);
    void pwm_echo_callback(custom_interfaces::msg::Pwms::UniquePtr pwms);
    void dvl_vr_callback(custom_interfaces::msg::VR::UniquePtr vr);
    void dvl_drr_callback(custom_interfaces::msg::DRR::UniquePtr drr);
    void matlab_debug_received_callback(custom_interfaces::msg::Debug::UniquePtr message);

    void heartbeat_check_callback();
    bool no_heartbeat(std::chrono::time_point<std::chrono::steady_clock> now, std::chrono::time_point<std::chrono::steady_clock> heartbeat_time);
    void control_mode_callback(std_msgs::msg::UInt8::UniquePtr msg);
    void ready_status_callback(std_msgs::msg::Bool::UniquePtr msg);

    void get_mux_mode_now();
    void get_ready_status_now();
    void set_mux_mode(int mode);
    void reset_drr();
    void reset_gyro();
    void reset_matlab_pos();
    void toggle_ready();
    void send_go_signal();
    void send_revive_signal();
    
    void refresh_display();
    void clear_display();
    std::string get_ping();

    bool thrust_interface_heartbeat = false;
    bool mux_heartbeat = false;
    bool cli_heartbeat = false;
    bool ctrl_heartbeat = false;
    bool echo_heartbeat = false;
    bool dvl_heartbeat = false;
    bool joystick_heartbeat = false;
    bool mission_manager_heartbeat = false;
    bool ping_ok = false;
    bool mission_manager_ready = false;

    int current_control_mode = 0; // 0 = Disabled, 1 = CLI, 2 = CTRL, 3 = Echo
    double seconds_since_ping = -1;
    double rtt = -1;
    PWM_Data pwms_cmd;
    PWM_Data pwms_cli;
    PWM_Data pwms_ctrl;
    PWM_Data pwms_echo;
    DVL_Data velocity;
    DVL_Data position;
    DVL_Data orientation;
    Debug_Message debug;

    rclcpp::Client<custom_interfaces::srv::ControlMode>::SharedPtr mux_client;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr reset_drr_client;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr reset_gyro_client;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr mission_manager_client;
    rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr force_pub_mux;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr force_pub_mission_manager;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr revive_pico_client;

    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr go_signal_publisher;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr matlab_pos_reset_publisher;

    rclcpp::TimerBase::SharedPtr heartbeat_timer;
    rclcpp::TimerBase::SharedPtr clear_display_timer;

    std::chrono::time_point<std::chrono::steady_clock> most_recent_thrust_interface_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_mux_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_cli_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_ctrl_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_echo_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_dvl_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_joystick_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_mission_manager_heartbeat;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_ping;
    std::chrono::time_point<std::chrono::steady_clock> most_recent_ping_attempt;
};


#endif