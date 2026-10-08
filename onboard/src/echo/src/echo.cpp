#include "echo.hpp"
#include <fcntl.h> // for fd stuff
#include <chrono>

using namespace std::chrono_literals;

Echo::Echo(std::unique_ptr<FD_Interface> fd, std::unique_ptr<TUI_Interface> tui) : 
    Node("echo"),
    log_fd(std::move(fd)),
    tui(std::move(tui)),
    state(Get_Command),
    write_log_active(false),
    publish_active(false),
    finished_reading(false)
     {

    pwm_received_subscription = this->create_subscription<custom_interfaces::msg::Pwms>(
        "pwm_cmd", 10, 
        std::bind(&Echo::pwm_received_subscription_callback, this, std::placeholders::_1));
    
    pwm_publisher = this->create_publisher<custom_interfaces::msg::Pwms>("pwm_echo", 10);

    heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("echo_heartbeat", 10);
    heartbeat_timer = this->create_wall_timer(500ms, 
            std::bind(&Echo::heartbeat_callback, this));
        
    refresh_display();
}

void Echo::pwm_received_subscription_callback(custom_interfaces::msg::Pwms::UniquePtr pwms_msg) {
    if (write_log_active) {
        log_pwms(pwms_msg->pwms);
    }
}

void Echo::log_pwms(std::array<int32_t,8> pwms) {
    char log_message[5] = {0}; // PWM = 4 chars, space = 1 char
    for (auto pwm : pwms) {
        sprintf(log_message, "%d ", pwm);
        if (write(log_fd->get_write_fd(), log_message, 5) < 5) {
            RCLCPP_WARN(this->get_logger(), "Unable to write to PWM log file");
        }
    }
    if (write(log_fd->get_write_fd(), "\n",  1) < 1) {
        RCLCPP_WARN(this->get_logger(), "Unable to write to PWM log file");
    }
}

void Echo::echo_pwms() { // TODO: Do error checking (currently assumes perfectly formatted file)
    heartbeat_callback(); // Don't skip PWMs at the start of the file!
    auto current_time = std::chrono::steady_clock::now();
    std::array<int32_t,8> pwms;
    custom_interfaces::msg::Pwms msg;
    char line[42] = {0}; // 4 * 8 pwms = 32, 8 spaces, 1 newline, 1 null character
    lseek(log_fd->get_read_fd(), 0, SEEK_SET);
    while (publish_active) {
        while (std::chrono::steady_clock::now() - current_time < 0.01s) {} // publish at 100 hz
        ssize_t num_read = read(log_fd->get_read_fd(), line, 41);
        if (num_read == 0) {
            finished_reading = true;
            break;
        } else if (num_read < 41) {
            RCLCPP_WARN(this->get_logger(), "Unable to read from PWM log file");
        }

        pwms = parse_log_line(line);
        msg.pwms = pwms;
        pwm_publisher->publish(msg);
        current_time = std::chrono::steady_clock::now();
    }
    msg.pwms = stop;
    pwm_publisher->publish(msg);
    refresh_display();
}

void Echo::heartbeat_callback() {
    std_msgs::msg::Empty msg;
    if (publish_active && !finished_reading) { // Only publish heartbeat if we're actively writing PWMs
        heartbeat_publisher->publish(msg);
    }
}

std::array<int32_t,8> Echo::parse_log_line(char* line) { // TODO: Do error checking (currently assumes perfectly formatted file)
    std::array<int32_t,8> pwms;
    size_t start_index = 41 - 6; // index of last PWM
    for (int i = 7; i >= 0; i--) {
        pwms[i] = std::stoi(line + start_index);
        line[start_index - 1] = '\0';
        start_index -= 5;
    }
    return pwms;
}

void Echo::invalid_command(std::string input, int index) {
    printf("Invalid command. Try again: ");
    fflush(stdout);
    while (input[index] != '\n') {
        index++;
    }
    tui->unfreeze_display();

}

void Echo::work_loop() {
    char mode = 0;
    int string_index = 0;

    tui->init_terminal();

    while (rclcpp::ok()) {
        tui->process_input();
        tui->freeze_display();
        std::string current_input = tui->get_current_input();
        mode = current_input[0];
        mode = tolower(mode);

        if (current_input.size() < 2) {
            printf("Invalid command. Try again: ");
            fflush(stdout);
            tui->unfreeze_display();
            continue; 
        }
        string_index = 1;

        if (state == Get_Command) {
            if (mode == 'e' || mode == 'q') {
                tui->unfreeze_display();
                break;
            }
            if (current_input[string_index] != '\n' || (mode != '0' && mode != '1')) {
                invalid_command(current_input, string_index);
                continue;
            }
            tui->unfreeze_display();
            state = (State)((int)(mode - '0' + 1));
            if (state == Write_Log) {
                write_log_active = true;
            }
            else if (state == Read_Log) {
                publish_active = true;
                std::thread echo_thread([&]() { // Needs to be seperate thread so that we can keep getting user input
                    echo_pwms();
                });
                echo_thread.detach();
            }
        }
        else if (state == Write_Log) {
            if (mode == 'e' || mode == 'q') {
                tui->unfreeze_display();
                write_log_active = false;
                state = Get_Command;
            }
            else {
                invalid_command(current_input, string_index);
                continue;
            }
        }
        else if (state == Read_Log) {
            if (mode == 'e' || mode == 'q') {
                tui->unfreeze_display();
                publish_active = false;
                finished_reading = false;
                state = Get_Command;
            }
            else {
                invalid_command(current_input, string_index);
                continue;
            }
        }
        else { // shouldn't ever happen
            tui->unfreeze_display();
        }
        refresh_display();
    }
    tui->restore_terminal();
    tui->clear_display();
}


Log_FD::Log_FD(std::string path) : Path_FD(path) {
    fd = open_file();
}

int Log_FD::open_file() {
    if ((fd = open(path.c_str(), O_CREAT | O_RDWR | O_APPEND, 0644)) == -1) {
        return -1;
    }
    return fd;
}

void Echo::refresh_display() {
    tui->clear_display();
    tui->refresh_display(2, state, finished_reading);
}

void Echo_TUI::display_tui(va_list args) {
    State current_state = (State)va_arg(args, int);
    bool finished_reading = (bool)va_arg(args, int);
    va_end(args);

    write(STDOUT_FILENO, "\x1B[5m", 4); // set blinking
    write(STDOUT_FILENO, "\x1B[1;7m", 6); // set bold, inverted
    switch (current_state) {
        case Write_Log:
            if (!finished_reading) {
                printf("====== Writing log to ~/LOG.txt... ======\n\n");
            }
            break;
        case Read_Log:
            if (!finished_reading) {
                printf("====== Reading from log at ~/LOG.txt... ======\n\n");
            } else {
                write(STDOUT_FILENO, "\x1B[0m", 4); // reset style
                write(STDOUT_FILENO, "\x1B[1;7m", 6); // set bold, inverted
                printf("========= Finished reading from file =========\n\n");
            }
            break;
        default:
            break;
    }
    write(STDOUT_FILENO, "\x1B[0m", 4); // reset style

    switch (current_state) {
        case Get_Command:
            printf("Select whether you would like to record or replay to/from the PWM log file.\n");
            printf("[0]: Record mode\n");
            printf("[1]: Replay mode\n");
            printf("[E]: Exit\n");
            printf("Mode: ");
            break;
        case Write_Log:
            printf("Type 'E' when you are done (program will not exit): ");
            break;
        case Read_Log:
            printf("Type 'E' to stop reading (program will not exit): ");
            break;
        default:
            break;
    }

    fflush(stdout);
}

#ifndef ENABLE_TESTING

int main(int argc, char* argv[]) {
    std::string filepath = getenv("HOME");
    filepath += "/LOG.txt";
    std::unique_ptr<FD_Interface> log_fd = std::make_unique<Log_FD>(filepath);
    std::unique_ptr<TUI_Interface> tui = std::make_unique<Echo_TUI>();

    rclcpp::init(argc, argv);
    auto echo = std::make_shared<Echo>(std::move(log_fd), std::move(tui));

    std::thread ros_thread([&]() { // Needs to be seperate thread so that we can get user input from main()
        rclcpp::spin(echo);
    });

    echo->work_loop();
    
    rclcpp::shutdown();
    ros_thread.join();
    
    return 0;
}
#endif // ENABLE_TESTING

