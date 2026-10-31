#include "tui.hpp" 

using namespace std::chrono_literals;

Dashboard::Dashboard(std::unique_ptr<TUI_Interface> tui) : Node("tui"), tui(std::move(tui)) {
    // Heartbeats
    thrust_interface_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("thrust_interface_heartbeat", 10, 
        std::bind(&Dashboard::thrust_interface_heartbeat_received_callback, this, std::placeholders::_1));
    mux_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("mux_heartbeat", 10, 
        std::bind(&Dashboard::mux_heartbeat_received_callback, this, std::placeholders::_1));
    cli_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("cli_heartbeat", 10, 
        std::bind(&Dashboard::cli_heartbeat_received_callback, this, std::placeholders::_1));
    ctrl_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("ctrl_heartbeat", 10, 
        std::bind(&Dashboard::ctrl_heartbeat_received_callback, this, std::placeholders::_1));
    echo_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("echo_heartbeat", 10, 
        std::bind(&Dashboard::echo_heartbeat_received_callback, this, std::placeholders::_1));
    dvl_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("dvl_heartbeat", 10, 
        std::bind(&Dashboard::dvl_heartbeat_received_callback, this, std::placeholders::_1));
    joystick_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("joystick_heartbeat", 10, 
        std::bind(&Dashboard::joystick_heartbeat_received_callback, this, std::placeholders::_1));
    mission_manager_heartbeat_subscription = this->create_subscription<std_msgs::msg::Empty>("mission_manager_heartbeat", 10, 
        std::bind(&Dashboard::mission_manager_heartbeat_received_callback, this, std::placeholders::_1));
    
    // Data
    pwm_cmd_subscription = this->create_subscription<custom_interfaces::msg::Pwms>("pwm_cmd", 10, 
        std::bind(&Dashboard::pwm_cmd_callback, this, std::placeholders::_1));
    pwm_cli_subscription = this->create_subscription<custom_interfaces::msg::Pwms>("pwm_cli", 10, 
        std::bind(&Dashboard::pwm_cli_callback, this, std::placeholders::_1));
    pwm_ctrl_subscription = this->create_subscription<custom_interfaces::msg::Pwms>("pwm_ctrl", 10, 
        std::bind(&Dashboard::pwm_ctrl_callback, this, std::placeholders::_1));
    pwm_echo_subscription = this->create_subscription<custom_interfaces::msg::Pwms>("pwm_echo", 10, 
        std::bind(&Dashboard::pwm_echo_callback, this, std::placeholders::_1));
    dvl_vr_subscription = this->create_subscription<custom_interfaces::msg::VR>("velocity_report", 10, 
        std::bind(&Dashboard::dvl_vr_callback, this, std::placeholders::_1));
    dvl_drr_subscription = this->create_subscription<custom_interfaces::msg::DRR>("dead_reck_report", 10, 
        std::bind(&Dashboard::dvl_drr_callback, this, std::placeholders::_1));
    mission_status_subscription = this->create_subscription<custom_interfaces::msg::MissionStatus>("mission_status", 10,
        std::bind(&Dashboard::mission_status_received_callback, this, std::placeholders::_1));
    current_control_mode_subscription = this->create_subscription<std_msgs::msg::UInt8>("current_mode", 10, 
        std::bind(&Dashboard::control_mode_callback, this, std::placeholders::_1));
    ready_status_subscription = this->create_subscription<std_msgs::msg::Bool>("current_ready_state", 10, 
        std::bind(&Dashboard::ready_status_callback, this, std::placeholders::_1));
    gamepad_subscription = this->create_subscription<custom_interfaces::msg::Gamepad>("ps5_controller", 10,
        std::bind(&Dashboard::gamepad_received_callback, this, std::placeholders::_1));

    // matlab debug
    matlab_debug_flag_0_subscription = this->create_subscription<custom_interfaces::msg::DebugFlag>("matlab_debug_flag_0", 10,
        std::bind(&Dashboard::matlab_debug_f0_received_callback, this, std::placeholders::_1));
    matlab_debug_flag_1_subscription = this->create_subscription<custom_interfaces::msg::DebugFlag>("matlab_debug_flag_1", 10,
        std::bind(&Dashboard::matlab_debug_f1_received_callback, this, std::placeholders::_1));
    matlab_debug_flag_2_subscription = this->create_subscription<custom_interfaces::msg::DebugFlag>("matlab_debug_flag_2", 10,
        std::bind(&Dashboard::matlab_debug_f2_received_callback, this, std::placeholders::_1));
    matlab_debug_actual_pt_subscription = this->create_subscription<custom_interfaces::msg::DebugState>("matlab_debug_actual_pt", 10,
        std::bind(&Dashboard::matlab_debug_actual_pt_received_callback, this, std::placeholders::_1));
    matlab_debug_set_pt_subscription = this->create_subscription<custom_interfaces::msg::DebugState>("matlab_debug_set_pt", 10,
        std::bind(&Dashboard::matlab_debug_set_pt_received_callback, this, std::placeholders::_1));
        
    go_signal_publisher = this->create_publisher<std_msgs::msg::Bool>("go_signal", 10);
    matlab_pos_reset_publisher = this->create_publisher<std_msgs::msg::Bool>("matlab_pos_reset", 10);

    heartbeat_timer = this->create_wall_timer(17ms,
            std::bind(&Dashboard::heartbeat_check_callback, this)); // 60 hz
    
    clear_display_timer = this->create_wall_timer(5s,
            std::bind(&Dashboard::clear_display, this)); // Screen resizing, etc. will get taken care of every 5 seconds

    mux_client = this->create_client<custom_interfaces::srv::ControlMode>("control_mode");
    reset_drr_client = this->create_client<std_srvs::srv::Trigger>("reset_drr");
    reset_gyro_client = this->create_client<std_srvs::srv::Trigger>("reset_gyro");
    mission_manager_client = this->create_client<std_srvs::srv::Trigger>("ready_signal_service");
    force_pub_mux = this->create_client<std_srvs::srv::SetBool>("force_pub");
    force_pub_mission_manager = this->create_client<std_srvs::srv::Trigger>("pub_ready_status");
    revive_pico_client = this->create_client<std_srvs::srv::Trigger>("revive_pico");
    
    refresh_display();
}

void Dashboard::get_mux_mode_now() {
    std::shared_ptr<std_srvs::srv::SetBool::Request> request = std::make_shared<std_srvs::srv::SetBool::Request>();
    request->data = true;
    force_pub_mux->async_send_request(request);
}

void Dashboard::get_ready_status_now() {
    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    force_pub_mission_manager->async_send_request(request);
}

void Dashboard::heartbeat_check_callback() {
    auto current_time = std::chrono::steady_clock::now();
    thrust_interface_heartbeat = no_heartbeat(current_time, most_recent_thrust_interface_heartbeat);
    mux_heartbeat = no_heartbeat(current_time, most_recent_mux_heartbeat);
    cli_heartbeat = no_heartbeat(current_time, most_recent_cli_heartbeat);
    ctrl_heartbeat = no_heartbeat(current_time, most_recent_ctrl_heartbeat);
    echo_heartbeat = no_heartbeat(current_time, most_recent_echo_heartbeat);
    dvl_heartbeat = no_heartbeat(current_time, most_recent_dvl_heartbeat);
    joystick_heartbeat = no_heartbeat(current_time, most_recent_joystick_heartbeat);  
    mission_manager_heartbeat = no_heartbeat(current_time, most_recent_mission_manager_heartbeat); 
    refresh_display();
}

bool Dashboard::no_heartbeat(std::chrono::time_point<std::chrono::steady_clock> now, std::chrono::time_point<std::chrono::steady_clock> heartbeat_time) {
    return ((now - heartbeat_time) <= 1s);
}

void Dashboard::thrust_interface_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_thrust_interface_heartbeat = std::chrono::steady_clock::now();
    thrust_interface_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::mux_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_mux_heartbeat = std::chrono::steady_clock::now();
    if (!mux_heartbeat) {
        mux_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
        get_mux_mode_now();
    }
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::cli_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_cli_heartbeat = std::chrono::steady_clock::now();
    cli_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::ctrl_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_ctrl_heartbeat = std::chrono::steady_clock::now();
    ctrl_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}


void Dashboard::echo_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_echo_heartbeat = std::chrono::steady_clock::now();
    echo_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::dvl_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_dvl_heartbeat = std::chrono::steady_clock::now();
    dvl_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}


void Dashboard::joystick_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_joystick_heartbeat = std::chrono::steady_clock::now();
    joystick_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::mission_manager_heartbeat_received_callback(std_msgs::msg::Empty::UniquePtr heartbeat) {
    most_recent_mission_manager_heartbeat = std::chrono::steady_clock::now();
    if (!mission_manager_heartbeat) {
        mission_manager_heartbeat = true; // If we just received a heartbeat, then we certainly have a heartbeat!
        get_ready_status_now();
    }
    (void)heartbeat; // stop compiler complaining
}

void Dashboard::pwm_cmd_callback(custom_interfaces::msg::Pwms::UniquePtr pwms) {
    pwms_cmd.pwms = pwms->pwms;
    pwms_cmd.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::pwm_cli_callback(custom_interfaces::msg::Pwms::UniquePtr pwms) {
    pwms_cli.pwms = pwms->pwms;
    pwms_cli.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::pwm_ctrl_callback(custom_interfaces::msg::Pwms::UniquePtr pwms) {
    pwms_ctrl.pwms = pwms->pwms;
    pwms_ctrl.timestamp = std::chrono::steady_clock::now();
}


void Dashboard::pwm_echo_callback(custom_interfaces::msg::Pwms::UniquePtr pwms) {
    pwms_echo.pwms = pwms->pwms;
    pwms_ctrl.timestamp = std::chrono::steady_clock::now();
}


void Dashboard::dvl_vr_callback(custom_interfaces::msg::VR::UniquePtr vr) {
    velocity.x = vr->velocity_data.x;
    velocity.y = vr->velocity_data.y;
    velocity.z = vr->velocity_data.z;
    velocity.altitude = vr->altitude;
    velocity.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::dvl_drr_callback(custom_interfaces::msg::DRR::UniquePtr drr) {
    position.x = drr->position.x;
    position.y = drr->position.y;
    position.z = drr->position.z;
    position.timestamp = std::chrono::steady_clock::now();
    orientation.x = drr->angle.x; // roll
    orientation.y = drr->angle.y; // pitch
    orientation.z = drr->angle.z; // roll
    orientation.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::mission_status_received_callback(custom_interfaces::msg::MissionStatus::UniquePtr status_msg) {
    for (int i = 0; i < 32; i ++) { // 32 = ROS message length. One less than MissionStatus C++ struct length.
        status.current_command[i] = status_msg->current_command_name[i];
        status.current_mission_file[i] = status_msg->current_mission_name[i];
    }
    status.duration_seconds = status_msg->seconds_total;
    status.seconds_remaining = status_msg->seconds_remaining;
    status.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::gamepad_received_callback(custom_interfaces::msg::Gamepad::UniquePtr gamepad_msg) {
    if (gamepad_msg->cross_button) {
        send_go_signal();
    }
}

void Dashboard::matlab_debug_f0_received_callback(custom_interfaces::msg::DebugFlag::UniquePtr msg) {
    for (int i = 0; i < 16; i++) { 
        debug_flag_0.name[i] = msg->name[i];
    }
    debug_flag_0.value = msg->value;
    debug_flag_0.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::matlab_debug_f1_received_callback(custom_interfaces::msg::DebugFlag::UniquePtr msg) {
    for (int i = 0; i < 16; i++) { 
        debug_flag_1.name[i] = msg->name[i];
    }
    debug_flag_1.value = msg->value;
    debug_flag_1.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::matlab_debug_f2_received_callback(custom_interfaces::msg::DebugFlag::UniquePtr msg) {
    for (int i = 0; i < 16; i++) { 
        debug_flag_2.name[i] = msg->name[i];
    }
    debug_flag_2.value = msg->value;
    debug_flag_2.timestamp = std::chrono::steady_clock::now();
}

void Dashboard::matlab_debug_actual_pt_received_callback(custom_interfaces::msg::DebugState::UniquePtr msg) {
    debug_position.x = msg->position.x;
    debug_position.y = msg->position.y;
    debug_position.z = msg->position.z;
    debug_position.timestamp = std::chrono::steady_clock::now();

    debug_orientation.x = msg->orientation.x; // roll
    debug_orientation.y = msg->orientation.y; // pitch
    debug_orientation.z = msg->orientation.z; // yaw
    debug_orientation.timestamp = std::chrono::steady_clock::now();

    debug_velocity.x = msg->velocity.x; // vx
    debug_velocity.y = msg->velocity.y; // vy
    debug_velocity.z = msg->velocity.z; // vz
    debug_velocity.timestamp = std::chrono::steady_clock::now();

    debug_angular_velocity.x = msg->angular_velocity.x; // p
    debug_angular_velocity.y = msg->angular_velocity.y; // q
    debug_angular_velocity.z = msg->angular_velocity.z; // r
    debug_angular_velocity.timestamp = std::chrono::steady_clock::now(); 
}

void Dashboard::matlab_debug_set_pt_received_callback(custom_interfaces::msg::DebugState::UniquePtr msg) {
    debug_position_setpt.x = msg->position.x;
    debug_position_setpt.y = msg->position.y;
    debug_position_setpt.z = msg->position.z;
    debug_position_setpt.timestamp = std::chrono::steady_clock::now();

    debug_orientation_setpt.x = msg->orientation.x; // roll
    debug_orientation_setpt.y = msg->orientation.y; // pitch
    debug_orientation_setpt.z = msg->orientation.z; // yaw
    debug_orientation_setpt.timestamp = std::chrono::steady_clock::now();

    debug_velocity_setpt.x = msg->velocity.x; // vx
    debug_velocity_setpt.y = msg->velocity.y; // vy
    debug_velocity_setpt.z = msg->velocity.z; // vz
    debug_velocity_setpt.timestamp = std::chrono::steady_clock::now();

    debug_angular_velocity_setpt.x = msg->angular_velocity.x; // p
    debug_angular_velocity_setpt.y = msg->angular_velocity.y; // q
    debug_angular_velocity_setpt.z = msg->angular_velocity.z; // r
    debug_angular_velocity_setpt.timestamp = std::chrono::steady_clock::now();
}


void Dashboard::control_mode_callback(std_msgs::msg::UInt8::UniquePtr msg) {
    current_control_mode = msg->data;
    refresh_display();
}

void Dashboard::ready_status_callback(std_msgs::msg::Bool::UniquePtr msg) {
    mission_manager_ready = msg->data;
    refresh_display();
}

void Dashboard::set_mux_mode(int mode) {
    if (mode == current_control_mode || !mux_heartbeat) {
        return;
    }
    std::shared_ptr<custom_interfaces::srv::ControlMode::Request> request = std::make_shared<custom_interfaces::srv::ControlMode::Request>();
    request->mode = mode;
    mux_client->async_send_request(request);
}

void Dashboard::reset_drr() {
    if (!dvl_heartbeat) {
        return;
    }
    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    reset_drr_client->async_send_request(request);
}

void Dashboard::reset_gyro() {
    if (!dvl_heartbeat) {
        return;
    }
    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    reset_gyro_client->async_send_request(request);
}

void Dashboard::reset_matlab_pos() {
    auto msg = std_msgs::msg::Bool();
    msg.data = true; 
    matlab_pos_reset_publisher->publish(msg);
}

void Dashboard::toggle_ready() {
    if (!mission_manager_heartbeat) {
        return;
    }
    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    mission_manager_client->async_send_request(request);
}

void Dashboard::send_go_signal() {
    auto msg = std_msgs::msg::Bool();
    msg.data = true;
    go_signal_publisher->publish(msg);
}

void Dashboard::send_revive_signal() {
    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    revive_pico_client->async_send_request(request);
}

void Dashboard::ping_loop() {
    while (rclcpp::ok()) {
        usleep(10);
        auto now = std::chrono::steady_clock::now();
        if ((now - most_recent_ping_attempt) < 1s) { // Only ping a 1hz
            continue;
        }
        
        if ((now - most_recent_ping_attempt) > 5s) { // Shouldn't need this because ping generally returns an error if disconnected, but this is here as a backup
            ping_ok = false;
        }

        std::string ping_output = get_ping();
        most_recent_ping_attempt = std::chrono::steady_clock::now(); // Don't re-use from a few lines up because pinging takes time
        if (ping_output == "error") {
            ping_ok = false;
            continue;
        }

        auto rtt_location = ping_output.find("rtt"); // Location of the string "rtt", not the value of rtt
        if (rtt_location == std::string::npos || (rtt_location + 28 > ping_output.length())) { // bad message
            continue;
        }
        auto rtt_string = ping_output.substr(rtt_location + 23, 5); // extract value of rtt
        rtt = stod(rtt_string);
        most_recent_ping = most_recent_ping_attempt; // If we got here, it's a successful ping
        ping_ok = true;
    }
}

void Dashboard::work_loop() {
    tui->init_terminal();
    tui->clear_display();
    while (rclcpp::ok()) {
        bool exit = false;
        usleep(10);
        tui->process_input();
        std::string input = tui->get_current_input();
        if (input.find("\n") != std::string::npos) { // user pressed enter
            if (input.length() != 2) { // all valid input will be of form [char]\n
                tui->clear_input();
                continue;
            }
            char input_char = tolower(input[0]);
            switch (input_char) {
                case '0':
                case '1':
                case '2':
                case '3':
                    set_mux_mode((int)(input_char - '0'));
                    break;
                case 'a':
                    reset_drr();
                    break;
                case 'b':
                    reset_gyro();
                    break;
                case 'c':
                    reset_matlab_pos();
                    break;
                case 'd':
                    toggle_ready();
                    break;
                case 'e':
                    send_go_signal();
                    break;
                case 'f':
                    send_revive_signal();
                    break;
                case 'q':
                    exit = true;
                    break;
                default:
                    break;
            }
            tui->clear_input();
        }
        if (exit) {
            break;
        }
    }

    tui->restore_terminal();
    tui->clear_display();
}

void Dashboard::refresh_display() {
    auto now = std::chrono::steady_clock::now();
    seconds_since_ping = (double)((now - most_recent_ping) / 1ms) / (double)1000;
    tui->refresh_display(32, current_control_mode,
                             thrust_interface_heartbeat,
                             mux_heartbeat,
                             cli_heartbeat,
                             ctrl_heartbeat,
                             echo_heartbeat,
                             dvl_heartbeat,
                             joystick_heartbeat,
                             mission_manager_heartbeat,
                             pwms_cmd,
                             pwms_cli,
                             pwms_ctrl,
                             pwms_echo,
                             velocity,
                             position,
                             orientation,
                             status,
                             debug_flag_0,
                             debug_flag_1,
                             debug_flag_2,
                             debug_position,
                             debug_orientation,
                             debug_velocity,
                             debug_angular_velocity,
                             debug_position_setpt,
                             debug_orientation_setpt,
                             debug_velocity_setpt,
                             debug_angular_velocity_setpt,
                             ping_ok,
                             rtt,
                             seconds_since_ping,
                             mission_manager_ready);
}
void Dashboard::clear_display() {
    tui->clear_display();
}

void Dashboard_TUI::process_input() {
    display_mutex.lock();
    int c = 0;
    while (read(STDIN_FILENO, &c, 1) != 0) {
        if ((c >= 32 && c <= 126) || c == '\n') {
            current_input.push_back(c);
            cursor_pos += 1;
        }
        if (c == 127 && cursor_pos > 0) {
            current_input.pop_back();
            cursor_pos -= 1;
        }
    }
    display_mutex.unlock();
}

void Dashboard_TUI::refresh_display(int numArgs, ...) {
    display_mutex.lock();

    va_list args;
    va_start(args, numArgs);

    tcflush(STDIN_FILENO, TCIFLUSH);
    display_tui(args);
    display_mutex.unlock();
}

void Dashboard_TUI::jump_to_column(int col_number) {
    std::string column_string = "\x1B[" + std::to_string(col_number) + "G";
    write(STDOUT_FILENO, column_string.c_str(), column_string.length());
}

void Dashboard_TUI::reset_cursor_pos() {
    write(STDOUT_FILENO, "\x1B[H", 3);
}

void Dashboard_TUI::write_header(std::string header, int col_number) {
    jump_to_column(col_number);
    write(STDOUT_FILENO, "\x1B[1m", 4); // set bold
    printf("%s\n", header.c_str());
    write(STDOUT_FILENO, "\x1B[22m", 5); // unset bold
    jump_to_column(col_number);
}

void Dashboard_TUI::write_selectable_header(std::string header, int current_mode, int active_mode, const int col_number) {
    if (active_mode == current_mode) {
        write_header("= " + header + " =", col_number);
    }
    else {
        write_header(header + "    ", col_number); // extra spaces to erase = = from when selected
    }
}

void Dashboard_TUI::display_status_with_colour(bool heartbeat, const int col_number, std::string colour_string) {
    jump_to_column(col_number);
    if (!heartbeat) {
        write(STDOUT_FILENO, "\x1B[5;7m", 6); // blinking, inverted
        write(STDOUT_FILENO, colour_string.c_str(), colour_string.length()); // set colour
        printf("== No heartbeat detected! ==\n");
        write(STDOUT_FILENO, "\x1B[0m", 4); // reset style
    } else {
        printf("============ OK ============\n");
    }
    jump_to_column(col_number);
}

void Dashboard_TUI::display_blue_if_fresh(std::chrono::time_point<std::chrono::steady_clock> timestamp, std::string message){
    fflush(stdout);
    if (fresh_evaluation_time - timestamp < 0.5s) {
        write(STDOUT_FILENO, "\x1B[1;94m", 7); // bold, blue
    }
    printf("%s", message.c_str());
    fflush(stdout);
    write(STDOUT_FILENO, "\x1B[39;22m", 8); // unset bold, blue
    fflush(stdout);
}

void Dashboard_TUI::display_warning_status(bool heartbeat, const int col_number) {
    display_status_with_colour(heartbeat, col_number, "\x1B[38;5;166;107m"); // orange foreground, white background
}

void Dashboard_TUI::display_critical_status(bool heartbeat, const int col_number) {
    display_status_with_colour(heartbeat, col_number, "\x1B[31;107m"); // red foreground, white background
}

void Dashboard_TUI::display_noncritical_status(bool heartbeat, const int col_number) {
    display_status_with_colour(heartbeat, col_number, ""); // no special colour (black foreground, white background)
}

void Dashboard_TUI::display_escalatable_critical_status(int current_mode, int critical_mode, bool heartbeat, const int col_number) {
    if (current_mode == critical_mode) {
        display_critical_status(heartbeat, col_number);
    }
    else {
        display_noncritical_status(heartbeat, col_number);
    }
}

void Dashboard_TUI::display_escalatable_warning_status(int current_mode, int warning_mode, bool heartbeat, const int col_number) {
    if (current_mode == warning_mode) {
        display_warning_status(heartbeat, col_number);
    }
    else {
        display_noncritical_status(heartbeat, col_number);
    }
}

void Dashboard_TUI::display_mission_manager(int current_mux_mode, int warning_mode, bool heartbeat, bool ready, const int col_number) {
    if (!heartbeat || !ready) {
        display_escalatable_warning_status(current_mux_mode, warning_mode, heartbeat, col_number);
    } else {
        display_ready(col_number);
    }
}

void Dashboard_TUI::display_ready(const int col_number) {
    jump_to_column(col_number);
    write(STDOUT_FILENO, "\x1B[48;5;34;97m", 13); // set green background, white foreground
    printf("=========== Ready ==========\n");
    write(STDOUT_FILENO, "\x1B[0m", 4); // reset style
}


/* Note: this function returns cursor to its original position so sub-columns can be next to each other */
void Dashboard_TUI::display_pwms(std::array<int,8> pwms, const int col_number, std::chrono::time_point<std::chrono::steady_clock> timestamp) {
    write(STDOUT_FILENO, "\x1B[s", 3); // Save cursor position
    printf("\n");
    if (pwms[0] == 0) { // no pwms to display
        for (int i = 0; i < 8; i ++) {
            jump_to_column(col_number);
            printf("|\n");
        }
        write(STDOUT_FILENO, "\x1B[u", 3); // Restore cursor position
        return;
    }
    for (int i = 0; i < 8; i++) {
        jump_to_column(col_number);
        printf("| ");
        char buf[32] = {0};
        snprintf(buf, 32, "%d", pwms[i]);
        display_blue_if_fresh(timestamp, std::string(buf));
        printf("\n");
    }
    write(STDOUT_FILENO, "\x1B[u", 3); // Restore cursor position
}

void Dashboard_TUI::fill_right_col(const int col_number) {
    for (int i = 0; i < 9; i++) {
        jump_to_column(col_number);
        printf("|\n");
    }
}

void Dashboard_TUI::display_all_pwms(PWM_Data pwms_cmd, PWM_Data pwms_cli, PWM_Data pwms_ctrl, PWM_Data pwms_echo) {
    printf("| - CMD - ");
    display_pwms(pwms_cmd.pwms, col_2, pwms_cmd.timestamp);
    jump_to_column(col_2_1);
    printf("| - CLI - ");
    display_pwms(pwms_cli.pwms, col_2_1, pwms_cli.timestamp);
    jump_to_column(col_2_2);
    printf("| - CTRL - ");
    display_pwms(pwms_ctrl.pwms, col_2_2, pwms_ctrl.timestamp);
    jump_to_column(col_2_3);
    printf("| - ECHO - |");
    display_pwms(pwms_echo.pwms, col_2_3, pwms_echo.timestamp);
    fill_right_col(col_2_4);
}

void Dashboard_TUI::display_connection_info(bool connection_ok, double seconds_since_ping, double ping_rtt, const int col_number) {
    jump_to_column(col_number);
    if (!connection_ok) {
        write(STDOUT_FILENO, "\x1B[5;7m", 6); // blinking, inverted
        write(STDOUT_FILENO, "\x1B[31;107m", 9); // set red foreground, white background
        printf("========== No robot connection! Check ethernet cable. =========\n");
        write(STDOUT_FILENO, "\x1B[0m", 4); // reset style
    }
    else {
        printf("Most recent ping: rtt %07.3lfms | %04.1lf seconds since last ping.\n", ping_rtt, seconds_since_ping);
    }
}

std::string Dashboard_TUI::format_floating_point(double coordinate) {
    char buf[11] = {0};
    if (fabs(coordinate) > 999) { // impossibly large number
        snprintf(buf, 11, "%s", " Overflow ");
    } else {
        snprintf(buf, 11, "% 010.5f", coordinate);
    }
    return std::string(buf);
}

void Dashboard_TUI::display_vr(const int col_number, DVL_VR_Data velocity) {
    jump_to_column(col_number);
    printf("Velocity:    | x: ");
    display_blue_if_fresh(velocity.timestamp, format_floating_point(velocity.x));
    printf(" | y: ");
    display_blue_if_fresh(velocity.timestamp, format_floating_point(velocity.y));
    printf(" | z: ");
    display_blue_if_fresh(velocity.timestamp, format_floating_point(velocity.z));
    printf(" | alt: ");
    display_blue_if_fresh(velocity.timestamp, format_floating_point(velocity.altitude));
    printf(" |\n");
}


void Dashboard_TUI::display_drr(const int col_number, DVL_Data position, DVL_Data orientation) {
    jump_to_column(col_number);

    printf("Position:    | x: ");
    display_blue_if_fresh(position.timestamp, format_floating_point(position.x));
    printf(" | y: ");
    display_blue_if_fresh(position.timestamp, format_floating_point(position.y));
    printf(" | z: ");
    display_blue_if_fresh(position.timestamp, format_floating_point(position.z));
    printf(" |\n");

    jump_to_column(col_number);
    printf("Orientation: | x: ");
    display_blue_if_fresh(orientation.timestamp, format_floating_point(orientation.x));
    printf(" | y: ");
    display_blue_if_fresh(orientation.timestamp, format_floating_point(orientation.y));
    printf(" | z: ");
    display_blue_if_fresh(orientation.timestamp, format_floating_point(orientation.z));
    printf(" |\n");
}

void Dashboard_TUI::display_commands() {
    printf("\n====================================== Commands ======================================\n");
    write(STDOUT_FILENO, "\x1B[s", 3); // Save cursor position
    printf("Mux Control:\n");
    printf("[0]: Disabled\n");
    printf("[1]: CLI mode\n");
    printf("[2]: Matlab mode\n");
    printf("[3]: Echo mode\n");
    printf("\nEnter command (Q to quit) > ");
    fflush(stdout);

    write(STDOUT_FILENO, "\x1B[u", 3); // Restore cursor position
    jump_to_column(col_2);
    printf("Sensor Resets:\n");
    jump_to_column(col_2);
    printf("[A]: Reset DRR\n");
    jump_to_column(col_2);
    printf("[B]: Reset Gyro\n");
    jump_to_column(col_2);
    printf("[C]: Reset Matlab Pos\n");
    
    write(STDOUT_FILENO, "\x1B[u", 3); // Restore cursor position
    fflush(stdout);
    jump_to_column(col_2_3);
    printf("Robot Control:\n");
    jump_to_column(col_2_3);
    printf("[D]: Toggle Ready\n");
    jump_to_column(col_2_3);
    printf("[E]: Send Go Signal\n");
    jump_to_column(col_2_3);
    printf("[F]: Revive Pico\n");
}

void Dashboard_TUI::display_mission_status(const int col_number, Mission_Status status) {
    std::string current_mission_file = std::string(status.current_mission_file);
    std::string current_command = std::string(status.current_command);

    for (int i = current_mission_file.size(); i < 32; i++) {
        current_mission_file.push_back(' ');
    }
    for (int i = current_command.size(); i < 32; i++) {
        current_command.push_back(' ');
    }

    jump_to_column(col_number);
    printf("File: ");
    display_blue_if_fresh(status.timestamp, current_mission_file);
    printf("\n");

    jump_to_column(col_number);
    printf("Command: ");
    display_blue_if_fresh(status.timestamp, current_command);
    printf("\n");

    jump_to_column(col_number);
    printf("Total seconds: ");
    display_blue_if_fresh(status.timestamp, format_floating_point(status.duration_seconds));
    printf("\n");

    jump_to_column(col_number);
    printf("Seconds left: ");
    display_blue_if_fresh(status.timestamp, format_floating_point(status.seconds_remaining));
    printf("\n");
}

void Dashboard_TUI::display_debug(
    const int col_number, 
    int num_flags,
    Debug_Flag_Msg* flags, 
    Debug_State_Data* actual_point, 
    Debug_State_Data* set_point
) {
    for(int i = 0; i < num_flags; i++) {
        std::string name = std::string(flags[i].name);
        jump_to_column(col_number);
        printf("%-17s", (name + ":").c_str());
        display_blue_if_fresh(flags[i].timestamp, format_floating_point(flags[i].value));
        printf("\n");
    }

    std::unique_ptr<std::string[]> states(new std::string[]{
        "xi", "yi", "zi", 
        "roll", "pitch", "yaw",
        "vbx", "vby", "vbz",
        "wbx", "wby", "wbz"});
    
    printf("\n");
    jump_to_column(col_number + 9);
    printf("actual       setpt.\n\n");

    int state_idx = 0;
    for(int i = 0; i < 4; i++) {
        Debug_State_Data actual = actual_point[i];
        Debug_State_Data setpoint = set_point[i];

        jump_to_column(col_number);
        printf("%-7s", (states[state_idx++] + ":").c_str());
        display_blue_if_fresh(actual.timestamp, format_floating_point(actual.x));
        printf(" | ");
        jump_to_column(col_number + 7 + 10);
        display_blue_if_fresh(setpoint.timestamp, format_floating_point(setpoint.x));
        printf("\n");

        jump_to_column(col_number);
        printf("%-7s", (states[state_idx++] + ":").c_str());
        display_blue_if_fresh(actual.timestamp, format_floating_point(actual.y));
        printf(" | ");
        jump_to_column(col_number + 7 + 10);
        display_blue_if_fresh(setpoint.timestamp, format_floating_point(setpoint.y));
        printf("\n");

        jump_to_column(col_number);
        printf("%-7s", (states[state_idx++] + ":").c_str());
        display_blue_if_fresh(actual.timestamp, format_floating_point(actual.z));
        printf(" | ");
        jump_to_column(col_number + 7 + 10);
        display_blue_if_fresh(setpoint.timestamp, format_floating_point(setpoint.z));
        printf("\n");
    }
    write(STDOUT_FILENO, "\x1B[39;22m\n", 9);
}

std::string Dashboard::get_ping() { // TODO: when robot looses connection ping hangs for ~10 seconds. Break into seperate thread
    int pipe_fds[2];
    pipe(pipe_fds); // [0] = read end, [1] = write end
    int pid = fork();
    if (pid > 0) { // Parent
        char buf[512] = {0};
        int status;
        waitpid(pid, &status, 0);
        read(pipe_fds[0], buf, 512);
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        if (status != 0) {
            return "error";
        }
        return buf;
    } else { // Child
        dup2(pipe_fds[1], STDOUT_FILENO);
        dup2(pipe_fds[1], STDERR_FILENO);
        execlp("ping", "ping", "-c 1", "propulsion.local", NULL);
        close(pipe_fds[0]); // should never get here (or past here, i.e. to the return)
        close(pipe_fds[1]);
        return "error";
    }
}

void Dashboard_TUI::display_tui(va_list args) {
    int current_mux_mode = va_arg(args, int);
    bool thrust_interface_heartbeat = (bool)va_arg(args, int);
    bool mux_heartbeat = (bool)va_arg(args, int);
    bool cli_heartbeat = (bool)va_arg(args, int);
    bool ctrl_heartbeat = (bool)va_arg(args, int);
    bool echo_heartbeat = (bool)va_arg(args, int);
    bool dvl_heartbeat = (bool)va_arg(args, int);
    bool joystick_heartbeat = (bool)va_arg(args, int);
    bool mission_manager_heartbeat = (bool)va_arg(args, int);
    PWM_Data pwms_cmd = va_arg(args, PWM_Data);
    PWM_Data pwms_cli = va_arg(args, PWM_Data);
    PWM_Data pwms_ctrl = va_arg(args, PWM_Data);
    PWM_Data pwms_echo = va_arg(args, PWM_Data);
    DVL_VR_Data velocity = va_arg(args, DVL_VR_Data);
    DVL_Data position = va_arg(args, DVL_Data);
    DVL_Data orientation = va_arg(args, DVL_Data);
    Mission_Status status = va_arg(args, Mission_Status);

    Debug_Flag_Msg debug_flag_0 = va_arg(args, Debug_Flag_Msg);
    Debug_Flag_Msg debug_flag_1 = va_arg(args, Debug_Flag_Msg);
    Debug_Flag_Msg debug_flag_2 = va_arg(args, Debug_Flag_Msg);

    Debug_State_Data debug_position = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_orientation = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_velocity = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_angular_velocity = va_arg(args, Debug_State_Data);

    Debug_State_Data debug_position_setpt = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_orientation_setpt = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_velocity_setpt = va_arg(args, Debug_State_Data);
    Debug_State_Data debug_angular_velocity_setpt = va_arg(args, Debug_State_Data);

    bool ping_ok = (bool)va_arg(args, int);
    double ping_rtt = va_arg(args, double);
    double seconds_since_ping = va_arg(args, double);
    bool ready = (bool)va_arg(args, int);
    va_end(args);
    
    int num_flags = 3;
    Debug_Flag_Msg debug_flags[3] = {debug_flag_0, debug_flag_1, debug_flag_2};
    Debug_State_Data debug_states_actual[4] = {debug_position, debug_orientation, debug_velocity, debug_angular_velocity};
    Debug_State_Data debug_states_setpt[4] = {debug_position_setpt, debug_orientation_setpt, debug_velocity_setpt, debug_angular_velocity_setpt};
    fresh_evaluation_time = std::chrono::steady_clock::now();

    write(STDOUT_FILENO, "\x1B[?25l", 6); // invisible cursor
    reset_cursor_pos();

    write_header("thrust_interface", col_1);
    display_critical_status(thrust_interface_heartbeat, col_1);
    printf("\n");

    write_header("soft_mux", col_1);
    display_critical_status(mux_heartbeat, col_1);
    printf("\n");

    write_selectable_header("cli", current_mux_mode, 1, col_1);
    display_escalatable_critical_status(current_mux_mode, 1, cli_heartbeat, col_1);
    printf("\n");

    write_selectable_header("ctrl", current_mux_mode, 2, col_1);
    display_escalatable_critical_status(current_mux_mode, 2, ctrl_heartbeat, col_1);
    printf("\n");

    write_selectable_header("echo", current_mux_mode, 3, col_1);
    display_escalatable_critical_status(current_mux_mode, 3, echo_heartbeat, col_1);
    printf("\n");

    write_header("dvl", col_1);
    display_critical_status(dvl_heartbeat, col_1);
    display_vr(col_1, velocity);
    display_drr(col_1, position, orientation);
    printf("\n");

    write_header("Robot Connection Status", col_1);
    display_connection_info(ping_ok, seconds_since_ping, ping_rtt, col_1);

    printf("\n");
    printf("Systems in flashing red are critical and should be fixed immediately.\n");
    printf("Systems in flashing orange might be critical and should be fixed if needed.\n");
    printf("Systems in flashing white are not connected, but are noncritical and can probably be ignored.\n");
    printf("The current control mode is indicated by whether the mode is highlighted by = [mode] =.\n");
    printf("Fresh PWM and DVL data is shown bolded in blue. After 0.5s it becomes stale and returns to normal.\n ");

    display_commands();  
    
    reset_cursor_pos();

    write_header("PWMs", col_2);
    display_all_pwms(pwms_cmd, pwms_cli, pwms_ctrl, pwms_echo);
    printf("\n");

    write_header("Joystick", col_2);
    display_escalatable_critical_status(current_mux_mode, 2, joystick_heartbeat, col_2);
    printf("\n");

    write_header("mission_manager", col_2);
    display_mission_manager(current_mux_mode, 2, mission_manager_heartbeat, ready, col_2);

    reset_cursor_pos();

    write_header("Mission Status", col_3);
    display_mission_status(col_3, status);
    printf("\n");

    write_header("Matlab Debug", col_3);
    display_debug(col_3, num_flags, debug_flags, debug_states_actual, debug_states_setpt);

    write(STDOUT_FILENO, "\x1B[38;29H", 8); // jumpt to prompt (line/column)
    write(STDOUT_FILENO, "\x1B[?25h", 6); // visible cursor
    printf("%s", current_input.c_str());
    write(STDOUT_FILENO, "\x1B[0K", 4); // erase until end of line (removes backspaced characters)

    fflush(stdout);
}

// TODO: Add voltage, ability to reset dvl drr and gyro, mission manager heartbeat

int main(int argc, char* argv[]) {    
    rclcpp::init(argc, argv);
    std::unique_ptr<TUI_Interface> tui = std::make_unique<Dashboard_TUI>();
    auto dashboard = std::make_shared<Dashboard>(std::move(tui));
    
    std::thread ros_thread([&]() { // Needs to be seperate thread so that work loop can run
        rclcpp::spin(dashboard);
    });
    std::thread ping_thread([&]() { // Needs to be seperate thread so that ping delays don't freeze program
        dashboard->ping_loop();
    });

    dashboard->work_loop();

    rclcpp::shutdown();
    ros_thread.join();
    ping_thread.join();
    
    return 0;
}
