#ifndef ECHO_HPP
#define ECHO_HPP

#endif // ECHO_HPP

#include "rclcpp/rclcpp.hpp"
#include "custom_interfaces/msg/pwms.hpp"
#include "std_msgs/msg/empty.hpp"
#include "fd_interface.hpp"
#include "tui_interface.hpp"

using namespace rclcpp;

class Echo_TUI : public TUI_Interface {
    public:
        explicit Echo_TUI() {};
        virtual void display_tui(va_list args) override;
};

class Log_FD : public Path_FD {
protected:
    int open_file() override;
public:
    Log_FD(std::string path);
};

enum State {Get_Command=0, Write_Log=1, Read_Log=2};

class Echo : public Node {
public:
    Echo(std::unique_ptr<FD_Interface> fd, std::unique_ptr<TUI_Interface> tui);
    void work_loop();
private:
    std::unique_ptr<FD_Interface> log_fd;
    std::unique_ptr<TUI_Interface> tui;
    State state;
    bool write_log_active;
    bool publish_active;
    bool finished_reading;
    std::array<int32_t,8> stop = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

    Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_received_subscription;
    Publisher<custom_interfaces::msg::Pwms>::SharedPtr pwm_publisher;
    Publisher<std_msgs::msg::Empty>::SharedPtr heartbeat_publisher;
    rclcpp::TimerBase::SharedPtr heartbeat_timer;

    void pwm_received_subscription_callback(custom_interfaces::msg::Pwms::UniquePtr pwms_msg);
    void log_pwms(std::array<int32_t,8> pwms);
    void echo_pwms();
    void heartbeat_callback();
    void invalid_command(std::string input, int index);
    void refresh_display();
    std::array<int32_t,8> parse_log_line(char* line);
};