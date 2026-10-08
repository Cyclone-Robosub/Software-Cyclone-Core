#include "manipulator.hpp"
#include <vector>
#include <iostream>

using namespace std::chrono_literals;
using namespace rclcpp;

Manipulator::Manipulator(std::unique_ptr<FD_Interface> arduino_fd) :
    Node("manipulator"), 
    arduino_fd(std::move(arduino_fd))
     {
    command_received_subscription = this->create_subscription<std_msgs::msg::UInt8>(
        "manipulator_cmd", 10, 
        std::bind(&Manipulator::command_received_subscription_callback, this, std::placeholders::_1));
}

bool Manipulator::is_valid_command(uint8_t command) {
    return command == 1 || command == 2;
}
    
void Manipulator::command_received_subscription_callback(std_msgs::msg::UInt8::UniquePtr command) {
    uint8_t manipulator_command = command->data;
    if (is_valid_command(manipulator_command)) {
        send_command_to_arduino(manipulator_command + OFFSET_TO_ONE); // Convert raw number to ASCII
        usleep(2000000);  // 2000ms
        send_command_to_arduino('r'); // Go back to reset position
    }
}

void Manipulator::send_command_to_arduino(char command) {
    std::string serial_message = std::string(1,command) + "\r\n";
    int length = serial_message.size();
    
    ssize_t bytes_written = write(arduino_fd->get_write_fd(), serial_message.c_str(), length);
    
    if (bytes_written != length) {
        RCLCPP_WARN(this->get_logger(), 
                    "Failed to write complete to manipulator (wrote %zd/%d bytes)", 
                    bytes_written, length);
        arduino_fd->attempt_reconnect();
    }
}

Arduino_FD::Arduino_FD(std::string path) : Path_FD(path) {
    fd = open_file();
}

int Arduino_FD::open_file() {
    struct termios options;
    speed_t baud = 9600;
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
    std::unique_ptr<FD_Interface> fd = std::make_unique<Arduino_FD>("/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_34331323635351B01182-if00");
    rclcpp::init(argc, argv);
    auto manipulator = std::make_shared<Manipulator>(std::move(fd));
    rclcpp::spin(manipulator);
    
    rclcpp::shutdown();
    
    return 0;
}
#endif

