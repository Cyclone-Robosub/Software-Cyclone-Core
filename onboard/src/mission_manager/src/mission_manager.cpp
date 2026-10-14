#include "mission_manager.hpp"

#include <chrono>

using namespace std::chrono_literals;

MissionManager::MissionManager(std::unique_ptr<MissionFileParser> parser) :
    Node("mission_manager"),
    parser(std::move(parser)),
    status_received(false),
    command_successful(false),
    ready(false),
    missions_running(false) {
        command_publisher = this->create_publisher<custom_interfaces::msg::Command>("current_command", 10);
        toggle_ready_service =  this->create_service<std_srvs::srv::Trigger>("toggle_ready", std::bind(&MissionManager::trigger_ready_signal, this, std::placeholders::_1, std::placeholders::_2));
        ready_pub_service =  this->create_service<std_srvs::srv::Trigger>("pub_ready_status", std::bind(&MissionManager::pub_ready_status_from_service, this, std::placeholders::_1, std::placeholders::_2));
        go_signal_subscription= this->create_subscription<std_msgs::msg::Bool>("go_signal", 10, std::bind(&MissionManager::go_signal_callback, this, std::placeholders::_1));
        heartbeat_timer = this->create_wall_timer(500ms, std::bind(&MissionManager::heartbeat_callback, this));
        heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("mission_manager_heartbeat", 10);
        current_ready_status_publisher = this->create_publisher<std_msgs::msg::Bool>("current_ready_state", 10);
        // current_mission_status_publisher = this->create_publisher<std_msgs::msg::Bool>("mission_status", 10); // TODO: show current mission, command, and duration on dashboard
        // TODO: service to dynamically parse mission file
}

void MissionManager::work_loop() {
    while (rclcpp::ok()) {
        if (go) {
            go = false;
            missions_running = true; // TODO: mutex
            run_missions(); // NOTE: blocks thread
        }
        usleep(100); // Prevent hogging CPU. Probably a better solution exists. Maybe sleep this thread?
    }
}

void MissionManager::run_missions() {
    RCLCPP_INFO(this->get_logger(), "Running mission...");
    sleep(1000); // TODO REMOVE ME!!!
    if (missions_running) {
        RCLCPP_WARN(this->get_logger(), "Missions already running. Ignoring call to run_missions().");
        return;
    }

    missions = parser->get_parsed_missions();
    if (missions.size() < 1) {
        RCLCPP_WARN(this->get_logger(), "No files loaded! No missions will be run.");
        return;
    }

    for (std::shared_ptr<Root> mission_root : missions) {
        std::shared_ptr<Command> current_command = mission_root->get_next();
        while (!(current_command->mission_complete())) {
            auto current_command_msg = current_command->get_ros2_message();
            command_publisher->publish(*current_command_msg);
            while (!status_received) {
                // TODO: check timeout
                usleep(100); // Prevent hogging CPU. Probably a better solution exists. Maybe sleep this thread?
            }
            status_received = false;
            if (command_successful) {
                current_command = current_command->get_next();
            } else {
                current_command = current_command->get_failure();
            }
        }
    }
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
    if (missions_running) {
        RCLCPP_WARN(this->get_logger(), "Missions already running. Pause/resume functionality not implemented.");
        return;
    }
    if (ready) {
        go = true;
    } else {
        RCLCPP_WARN(this->get_logger(), "Go signal pressed but not ready. Ignoring.");
    }
}

void MissionManager::heartbeat_callback() {
    // TODO
}

#ifndef ENABLE_TESTING

/* We can optionally queue some mission files initially, and also load them dynamically afterwards via ROS2 calls */
int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    auto parser = std::make_unique<MissionFileParser>();
    parser->parse_files();

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
