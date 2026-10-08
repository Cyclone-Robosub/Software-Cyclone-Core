#include "thrust_interface.hpp"
#include <vector>

using namespace std::chrono_literals;
using namespace rclcpp;

Thrust_Interface::Thrust_Interface(std::vector<int> thrusters, 
                                   std::unique_ptr<FD_Interface> pico_fd, 
                                   int min_pwm, int max_pwm) :
    Node("thrust_interface"), 
    thrusters(thrusters), 
    pico_fd(std::move(pico_fd)),
    min_pwm(min_pwm), 
    max_pwm(max_pwm),
    no_heartbeat(true),
    no_pico(true)
     {
    
    pwm_received_subscription = this->create_subscription<custom_interfaces::msg::Pwms>(
        "pwm_cmd", 10, 
        std::bind(&Thrust_Interface::pwm_received_subscription_callback, this, std::placeholders::_1));

    heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("mux_heartbeat", 10, 
            std::bind(&Thrust_Interface::mux_heartbeat_received_callback, this, std::placeholders::_1));

    heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("thrust_interface_heartbeat", 10);
    
    kill_switch_service = this->create_service<std_srvs::srv::Trigger>("revive_pico", 
            std::bind(&Thrust_Interface::revive_pico, this, std::placeholders::_1, std::placeholders::_2));
    
    heartbeat_timer = this->create_wall_timer(500ms, 
            std::bind(&Thrust_Interface::heartbeat_callback, this));
}
    
void Thrust_Interface::pwm_received_subscription_callback(custom_interfaces::msg::Pwms::UniquePtr pwms_msg) {
    if (no_heartbeat) {
        return;
    }
    std::array<int, 8> pwms = pwms_msg->pwms;
    
    for (int i = 0; i < 8; i++) {
        if (pwms[i] != 0) {
            pwms[i] = std::max(pwms[i], min_pwm);
            pwms[i] = std::min(pwms[i], max_pwm);
        }
        send_pwm_to_pico(thrusters[i], pwms[i]);
    }
}

void Thrust_Interface::mux_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_heartbeat = std::chrono::steady_clock::now();
    if (no_heartbeat) {
        no_heartbeat = false; // If we just received a heartbeat, then we certainly have a heartbeat!
    }
    (void)heartbeat; // stop compiler complaining
}

void Thrust_Interface::evaluate_mux_heartbeat_freshness() {
    auto current_time = std::chrono::steady_clock::now();
    if (current_time - most_recent_heartbeat > 1s) {
        RCLCPP_INFO(this->get_logger(), "Didn't get heartbeat from mux. Sending stop command.");
        no_heartbeat = true;
        for (int i = 0; i < 8; i++) {
            send_pwm_to_pico(thrusters[i], 1500);
        }
    }
    else {
        no_heartbeat = false;
    }
}
void Thrust_Interface::send_heartbeat_to_pico() {
    std::string serial_message = "ping\n";
    int length = serial_message.size();
    
    serial_mutex.lock();
    ssize_t bytes_written = write(pico_fd->get_write_fd(), serial_message.c_str(), length);
    serial_mutex.unlock();
    
    if (bytes_written != length) {
        RCLCPP_WARN(this->get_logger(), 
                    "Failed to ping Pico (wrote %zd/%d bytes)", 
                    bytes_written, length);
        RCLCPP_WARN(this->get_logger(), "Attempting to reconnect to Pico."); // Just do it here so we only try every 1/2 second
        no_pico = true;
        pico_fd->attempt_reconnect();
    } else {
        no_pico = false;
    }
}

void Thrust_Interface::heartbeat_callback() {
    send_heartbeat_to_pico();
    evaluate_mux_heartbeat_freshness();
    send_heartbeat();
}

void Thrust_Interface::send_heartbeat() {
    if (!no_pico) {
        std_msgs::msg::Empty msg;
        this->heartbeat_publisher->publish(msg);
    }
}

void Thrust_Interface::revive_pico(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
    (void)request; // stop compiler complaining
    std::string serial_message = "resume from kill\n";
    int length = serial_message.size();

    serial_mutex.lock();
    ssize_t bytes_written = write(pico_fd->get_write_fd(), serial_message.c_str(), length);
    serial_mutex.unlock();
    if (bytes_written != length) {
        response->success = false;
        RCLCPP_WARN(this->get_logger(), 
                    "Failed to write complete message to revive Pico. It might still be killed. Wrote %zd/%d bytes.", 
                    bytes_written, length);
        return;
    }
    // check that Pico responded with success;
    char response_buf[32] = {0};
    read(pico_fd->get_read_fd(), response_buf, 32);
    if (strncmp("revived", response_buf, 7)) { // if we didn't get "revived" from Pico
        response->success = false;
        RCLCPP_WARN(this->get_logger(), 
                    "Successfully wrote revive message to Pico, but did not receive successful response from Pico.");
        return;
    }
    response->success = true;
}

void Thrust_Interface::send_pwm_to_pico(int thruster, int pwm) {
    std::string serial_message = "Set " + std::to_string(thruster) + " PWM " + std::to_string(pwm) + "\n";
    int length = serial_message.size();
    
    serial_mutex.lock();
    ssize_t bytes_written = write(pico_fd->get_write_fd(), serial_message.c_str(), length);
    serial_mutex.unlock();
    
    if (bytes_written != length) {
        RCLCPP_WARN(this->get_logger(), 
                    "Failed to write complete message for thruster %d (wrote %zd/%d bytes)", 
                    thruster, bytes_written, length);
        no_pico = true;
    } else {
        no_pico = false;
    }
}

Pico_FD::Pico_FD(std::string path) : Path_FD(path) {
    fd = open_file();
}

int Pico_FD::open_file() {
    struct termios options;
    speed_t baud = 115200;
    int status, fd;
    
    if ((fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1) {
        return -1;
    }
    
    fcntl(fd, F_SETFL, O_RDWR);
    
    // Get and modify current options:
    tcgetattr(fd, &options);
    cfmakeraw(&options);
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 1;  // 1/10 (1 decisecond)
    
    tcsetattr(fd, TCSANOW, &options);
    
    ioctl(fd, TIOCMGET, &status);
    status |= TIOCM_DTR;
    status |= TIOCM_RTS;
    ioctl(fd, TIOCMSET, &status);
    
    usleep(100000);  // 100ms
    
    return fd;
}

#ifndef ENABLE_TESTING

int main(int argc, char* argv[]) {
    std::vector<int> thrusters = {2, 0, 6, 4, 19, 26, 21, 27};
    std::unique_ptr<FD_Interface> fd = std::make_unique<Pico_FD>("/dev/serial/by-id/usb-MicroPython_Board_in_FS_mode_7327d9a2ecd31892-if00");
    rclcpp::init(argc, argv);
    auto thrust_interface = std::make_shared<Thrust_Interface>(thrusters, std::move(fd), 1100, 1900);
    rclcpp::spin(thrust_interface);
    
    rclcpp::shutdown();
    
    return 0;
}
#endif

