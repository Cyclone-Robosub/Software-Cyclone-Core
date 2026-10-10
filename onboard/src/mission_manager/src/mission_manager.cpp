#include "mission_manager.hpp"

MissionManager::MissionManager(std::unique_ptr<MissionFileParser> parser) :
    Node("mission_manager"), parser(std::move(parser)) {
        command_publisher = this->create_publisher<custom_interfaces::msg::Command>("current_command", 10);
        // TODO: service to toggle ready status
        // TODO: service to force pub current ready status
        // TODO: service to dynamically parse mission file
        // TODO: service for go signal
}

void MissionManager::run_missions() {
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

#ifndef ENABLE_TESTING

/* We can optionally queue some mission files initially, and also load them dynamically afterwards via ROS2 calls */
int main(int argc, char * argv[]) {
    auto parser = std::make_unique<MissionFileParser>();
    parser->parse_files();

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MissionManager>(std::move(parser)));
    rclcpp::shutdown();
    return 0;
}

#endif // ENABLE_TESTING
