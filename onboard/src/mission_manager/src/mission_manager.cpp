#include "mission_manager.hpp"

#include <chrono>

using namespace std::chrono_literals;

MissionManager::MissionManager(std::unique_ptr<MissionFileParser> parser) :
    Node("mission_manager"),
    parser(std::move(parser)),
    status_received(false),
    command_successful(false),
    ready(false),
    go(false),
    mission_running(false),
    cancel_mission(false),
    timed_out(false),
    last_mission_status_update(std::chrono::steady_clock::now()) {
        command_publisher = this->create_publisher<custom_interfaces::msg::Command>("current_command", 10);
        heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("mission_manager_heartbeat", 10);
        current_ready_status_publisher = this->create_publisher<std_msgs::msg::Bool>("current_ready_state", 10);
        current_mission_status_publisher = this->create_publisher<custom_interfaces::msg::MissionStatus>("mission_status", 10);
        go_signal_subscription = this->create_subscription<std_msgs::msg::Bool>("go_signal", 10, std::bind(&MissionManager::go_signal_callback, this, std::placeholders::_1));
        command_result_subscription = this->create_subscription<custom_interfaces::msg::CommandResult>("command_result", 10, std::bind(&MissionManager::command_result_callback, this, std::placeholders::_1));
        toggle_ready_service =  this->create_service<std_srvs::srv::Trigger>("ready_signal_service", std::bind(&MissionManager::trigger_ready_signal, this, std::placeholders::_1, std::placeholders::_2));
        ready_pub_service =  this->create_service<std_srvs::srv::Trigger>("pub_ready_status", std::bind(&MissionManager::pub_ready_status_from_service, this, std::placeholders::_1, std::placeholders::_2));
        add_mission_file = this->create_service<custom_interfaces::srv::GetFile>("add_mission_file", std::bind(&MissionManager::add_mission_file_callback, this, std::placeholders::_1, std::placeholders::_2));
        add_mission_list = this->create_service<custom_interfaces::srv::GetFile>("add_mission_list", std::bind(&MissionManager::add_mission_list_callback, this, std::placeholders::_1, std::placeholders::_2));
        heartbeat_timer = this->create_wall_timer(500ms, std::bind(&MissionManager::heartbeat_callback, this));

        pub_ready_status();
}

void MissionManager::work_loop() {
    while (rclcpp::ok()) {
        if (go) {
            go = false;
            mission_running = true;
            initiate_mission_mutex.unlock();
            run_mission(); // NOTE: blocks thread
            initiate_mission_mutex.lock();
            mission_running = false;
            initiate_mission_mutex.unlock();
        }
        usleep(100); // Prevent hogging CPU. Probably a better solution exists. Maybe sleep this thread?
    }
}

void MissionManager::run_mission() {
    RCLCPP_INFO(this->get_logger(), "Running new mission.");
    if (!parser->new_mission_available()) {
        RCLCPP_WARN(this->get_logger(), "No missions available! Either no files were parsed, or all missions have been run.");
        return;
    }

    // First mission is root: call get_next() on that to get first real command
    std::shared_ptr<Command> current_command = parser->get_next_mission()->get_next();
    while (!(current_command->mission_complete()) && rclcpp::ok()) {
        auto current_command_msg = current_command->start_command_and_get_message();
        command_publisher->publish(*current_command_msg);
        current_command->custom_start_action();
        /* We loop until either:
         * - We receive a status update from MATLAB OR
         * - MATLAB takes too long to respond, so we time out ourselves OR
         * - The command meets seperate custom success criteria 
        */
        while (!status_received && rclcpp::ok()) {
            publish_mission_status_throttled(current_command, parser->get_current_mission_name());
            if (current_command->timeout_exceeded()) {
                status_mutex.lock();
                command_successful = false;
                timed_out = true;
                RCLCPP_WARN(this->get_logger(), "Command timed out. Treating as failed.");
                status_mutex.unlock();
                break;
            }
            if (current_command->custom_success_criteria_met()) {
                status_mutex.lock();
                command_successful = true;
                status_mutex.unlock();
                break;
            }
            if (cancel_mission) {
                auto idle_command_msg = std::make_shared<Idle>("cancel", -1)->get_ros2_message();
                command_publisher->publish(*idle_command_msg);
                return;
            }
            usleep(100); // Prevent hogging CPU. Probably a better solution exists. Maybe sleep this thread?
        }
        status_mutex.lock();
        status_received = false;
        if (command_successful) {
            current_command->custom_finish_action();
            current_command = current_command->get_next();
        } else {
            current_command = current_command->get_failure();
        }
        timed_out = false;
        status_mutex.unlock();
    }
    /* Finish command can be treated as an idle, so this publishes an idle after completing a mission */
    command_publisher->publish(*current_command->start_command_and_get_message());
    publish_mission_status_unthrottled(current_command, parser->get_current_mission_name());
    RCLCPP_INFO(this->get_logger(), "Mission complete!");
}

void MissionManager::pub_ready_status() {
    auto msg = std_msgs::msg::Bool();
    msg.data = ready;
    current_ready_status_publisher->publish(msg);
}

void MissionManager::trigger_ready_signal(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
    (void)request; // stop compiler complaining
    (void)response; // stop compiler complaining
    ready = !ready;
    pub_ready_status();
}

void MissionManager::pub_ready_status_from_service(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
    (void)request; // stop compiler complaining
    (void)response; // stop compiler complaining
    pub_ready_status();
}

void MissionManager::go_signal_callback(std_msgs::msg::Bool::SharedPtr signal) {
    /* Note that we don't use the value of signal. If go switch uROS is updated, we
     * could switch this to ::Empty or even to a trigger service. 
    */
    (void)signal; // stop compiler complaining
    initiate_mission_mutex.lock();
    if (mission_running) {
        cancel_mission = true;
        mission_running = false; // TODO: make sure that command_successful and status_received don't get set to true between cancel and next mission
        RCLCPP_INFO(this->get_logger(), "Mission was running, now cancelled.");
        initiate_mission_mutex.unlock();
        return;
    }
    if (ready) {
        go = true;
        // mutex is unlocked in work loop
    } else {
        RCLCPP_WARN(this->get_logger(), "Go signal pressed but not ready. Ignoring.");
        initiate_mission_mutex.unlock();
    }
}

void MissionManager::add_mission_file_callback(const std::shared_ptr<custom_interfaces::srv::GetFile::Request> request, std::shared_ptr<custom_interfaces::srv::GetFile::Response> response) {
    // Fail if filename doesn't end with '.json'
    if (request->file_name.size() < 5 || request->file_name.substr(request->file_name.size()-5) != ".json") {
        response->success = false;
        response->message = "Mission file " + request->file_name + " is not a .json file";
        return;
    }
    try {
        parser->add_file_to_parse(request->file_name);
        parser->parse_new_files();
    } catch (const std::invalid_argument& e) {
        response->success = false;
        response->message = "Parser could not read file" + request->file_name;
    }
    response->success = true;
    response->message = request->file_name + "parsed successfully";
}

void MissionManager::add_mission_list_callback(const std::shared_ptr<custom_interfaces::srv::GetFile::Request> request, std::shared_ptr<custom_interfaces::srv::GetFile::Response> response) {
    // Fail if filename doesn't end with '.txt'
    if (request->file_name.size() < 4 || request->file_name.substr(request->file_name.size()-4) != ".txt") {
        response->success = false;
        response->message = "Mission list " + request->file_name + " is not a .txt file";
        return;
    }
    try {
        parser->parse_mission_list_file(request->file_name);
        parser->parse_new_files();
    } catch (const std::invalid_argument& e) {
        response->success = false;
        response->message = "Parser could not read file" + request->file_name;
    }
    response->success = true;
    response->message = request->file_name + "parsed successfully";
}

void MissionManager::command_result_callback(custom_interfaces::msg::CommandResult::SharedPtr result) {
    status_mutex.lock();
    if (timed_out || !mission_running) {
        status_mutex.unlock();
        return;
    }
    if (result->state == result->SUCCESS) {
        command_successful = true;
    } else {
        command_successful = false;
    }
    status_received = true;
    status_mutex.unlock();
}

void MissionManager::heartbeat_callback() {
    auto msg = std_msgs::msg::Empty();
    heartbeat_publisher->publish(msg);
}

void MissionManager::publish_mission_status_throttled(std::shared_ptr<Command> current_command, std::string mission_name) {
    if (std::chrono::steady_clock::now() - last_mission_status_update < 33ms) { // 30 hz
        return;
    }
    publish_mission_status_unthrottled(current_command, mission_name);
}

void MissionManager::publish_mission_status_unthrottled(std::shared_ptr<Command> current_command, std::string mission_name) {
    last_mission_status_update = std::chrono::steady_clock::now();

    auto msg = custom_interfaces::msg::MissionStatus();
    msg.current_mission_name = string_to_array(mission_name);
    msg.current_command_name = string_to_array(current_command->get_name());
    msg.seconds_total = current_command->get_initial_duration_seconds();
    msg.seconds_remaining = current_command->get_seconds_remaining();
    current_mission_status_publisher->publish(msg);
}

std::array<uint8_t,32> MissionManager::string_to_array(std::string string) {
    std::array<uint8_t,32> array = {0};
    if (string.size() > 32) {
        RCLCPP_WARN(this->get_logger(), "String too long! Result will be truncated.");
    }
    for (int i = 0; i < 32; i++) {
        array[i] = string[i];
    }
    return array;
}

#ifndef ENABLE_TESTING

/* We can optionally queue some mission files initially, and also load them dynamically afterwards via ROS2 calls */
int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    auto parser = std::make_unique<MissionFileParser>();
    parser->parse_always_run_file();
    parser->parse_all_files();

    auto mission_manager = std::make_shared<MissionManager>(std::move(parser));
    
    std::thread ros_thread([&]() { // Needs to be seperate thread so that work loop can run
        rclcpp::spin(mission_manager);
    });

    mission_manager->work_loop();
    rclcpp::shutdown();

    ros_thread.join();

    return 0;
}

#endif // ENABLE_TESTING
