#include "soft_mux.hpp"


using namespace std::chrono_literals;


SoftMux::SoftMux() : rclcpp::Node("SoftMux"), control_mode(Disabled), no_ctrl_heartbeat(true), no_cli_heartbeat(true), no_echo_heartbeat(true) {
    //Inputs
    pwm_ctrl_subscriber = this->create_subscription<custom_interfaces::msg::Pwms>("pwm_ctrl", 10, std::bind(&SoftMux::pwm_ctrl_callback, this, std::placeholders::_1));
    pwm_cli_subscriber =  this->create_subscription<custom_interfaces::msg::Pwms>("pwm_cli", 10, std::bind(&SoftMux::pwm_cli_callback, this, std::placeholders::_1));
    pwm_echo_subscriber =  this->create_subscription<custom_interfaces::msg::Pwms>("pwm_echo", 10, std::bind(&SoftMux::pwm_echo_callback, this, std::placeholders::_1));
    ctrl_heartbeat_subscriber = this->create_subscription<std_msgs::msg::Empty>("ctrl_heartbeat", 10, std::bind(&SoftMux::ctrl_heartbeat_callback, this, std::placeholders::_1));
    cli_heartbeat_subscriber= this->create_subscription<std_msgs::msg::Empty>("cli_heartbeat",10, std::bind(&SoftMux::cli_heartbeat_callback, this, std::placeholders::_1));
    echo_heartbeat_subscriber= this->create_subscription<std_msgs::msg::Empty>("echo_heartbeat",10, std::bind(&SoftMux::echo_heartbeat_callback, this, std::placeholders::_1));


    //Heartbeat Timers
    heartbeat_timer = this->create_wall_timer(500ms, std::bind(&SoftMux::heartbeat_callback, this));
   
    //Services
    control_mode_service = this->create_service<custom_interfaces::srv::ControlMode>("control_mode", std::bind(&SoftMux::set_mode_srv, this, std::placeholders::_1));
    force_pub_service = this->create_service<std_srvs::srv::SetBool>("force_pub", std::bind(&SoftMux::pub_mode_srv, this, std::placeholders::_1, std::placeholders::_2));


    //Outputs
    pwm_cmd_publisher = this->create_publisher<custom_interfaces::msg::Pwms>("pwm_cmd", 10);
    current_control_mode_publisher = this->create_publisher<std_msgs::msg::UInt8>("current_mode", 10);
    mux_heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("mux_heartbeat", 10);
}




void SoftMux::pwm_ctrl_callback(custom_interfaces::msg::Pwms::UniquePtr pwm) {
    if (no_ctrl_heartbeat) {
        return;
    }
    if (control_mode == CTRL) {
        pwm_cmd_publish(std::move(pwm));
    }
}


void SoftMux::pwm_cli_callback(custom_interfaces::msg::Pwms::UniquePtr pwm) {
    if (no_cli_heartbeat) {
        return;
    }
    if (control_mode == CLI) {
        pwm_cmd_publish(std::move(pwm));
    }
}

void SoftMux::pwm_echo_callback(custom_interfaces::msg::Pwms::UniquePtr pwm) {
    if (no_echo_heartbeat) {
        return;
    }
    if (control_mode == Echo) {
        pwm_cmd_publish(std::move(pwm));
    }
}


void SoftMux::set_mode_srv(const std::shared_ptr<custom_interfaces::srv::ControlMode::Request> request) {
    this->control_mode = request->mode;
    publish_current_control_mode();
}

void SoftMux::pub_mode_srv(const std::shared_ptr<std_srvs::srv::SetBool::Request> request, std::shared_ptr<std_srvs::srv::SetBool::Response> response) {
    publish_current_control_mode();

    response->success = true;

    (void) request; // stop compiler complaining about unused variables
}

void SoftMux::publish_current_control_mode() {
    auto message = std_msgs::msg::UInt8();
    message.data = this->control_mode;
    this->current_control_mode_publisher->publish(message);
}



void SoftMux::pwm_cmd_publish(custom_interfaces::msg::Pwms::UniquePtr pwm) {
    this->pwm_cmd_publisher->publish(*(std::move(pwm)));
}


void SoftMux::ctrl_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    recent_ctrl_heartbeat = std::chrono::steady_clock::now();
    if (no_ctrl_heartbeat) {
        no_ctrl_heartbeat = false; // If we just received a heartbeat, then we certainly have a heartbeat!
    }
    (void) heartbeat; // stop compiler complaining about unused variables
}

void SoftMux::heartbeat_callback() {
    mux_heartbeat_send();
    ctrl_heartbeat_check();
    cli_heartbeat_check();
    echo_heartbeat_check();
    publish_stop_if_disabled();
}

void SoftMux::ctrl_heartbeat_check() {
    if (control_mode == CTRL) {
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - recent_ctrl_heartbeat > 1s) {
            RCLCPP_INFO(this->get_logger(), "Didn't get heartbeat from ctrl. Sending stop command.");
            no_ctrl_heartbeat = true;
            publish_stop_command();
        }
        else {
            no_ctrl_heartbeat = false;
        }
    }
}

void SoftMux::cli_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat){
    recent_cli_heartbeat = std::chrono::steady_clock::now();
    if (no_cli_heartbeat) {
        no_cli_heartbeat = false; // If we just received a heartbeat, then we certainly have a heartbeat!
    }
    (void) heartbeat; // stop compiler complaining about unused variables
}

void SoftMux::echo_heartbeat_check() {
    if (control_mode == Echo) {
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - recent_echo_heartbeat > 1s) {
            RCLCPP_INFO(this->get_logger(), "Didn't get heartbeat from echo. Sending stop command.");
            no_echo_heartbeat = true;
            publish_stop_command();
        }
        else {
            no_echo_heartbeat = false;
        }
    }
}

void SoftMux::echo_heartbeat_callback(std_msgs::msg::Empty::UniquePtr heartbeat){
    recent_echo_heartbeat = std::chrono::steady_clock::now();
    if (no_echo_heartbeat) {
        no_echo_heartbeat = false; // If we just received a heartbeat, then we certainly have a heartbeat!
    }
    (void) heartbeat; // stop compiler complaining about unused variables
}


void SoftMux::cli_heartbeat_check() {
    if (control_mode == CLI) {
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - recent_cli_heartbeat > 1s) {
            RCLCPP_INFO(this->get_logger(), "Didn't get heartbeat from cli. Sending stop command.");
            no_cli_heartbeat = true;
            publish_stop_command();
        }
        else {
            no_cli_heartbeat = false;
        }
    }  
}

void SoftMux::publish_stop_if_disabled() {
    if (control_mode == Disabled) {
        publish_stop_command();
    }
}


void SoftMux::mux_heartbeat_send() {
    std_msgs::msg::Empty msg;
    this->mux_heartbeat_publisher->publish(msg);
}


void SoftMux::publish_stop_command() {
    auto stop_pwms = custom_interfaces::msg::Pwms();
    for (int i = 0; i < 8; i++) {
        stop_pwms.pwms[i] = 1500;
    }
    pwm_cmd_publish(std::make_unique<custom_interfaces::msg::Pwms>(stop_pwms));
}


#ifndef ENABLE_TESTING
    int main(int argc, char* argv[]) {
        rclcpp::init(argc, argv);
        rclcpp::spin(std::make_shared<SoftMux>());
        rclcpp::shutdown();
        return 0;
    }
#endif

