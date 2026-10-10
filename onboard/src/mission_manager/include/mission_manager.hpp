#ifndef MISSION_MANAGER_HPP
#define MISSION_MANAGER_HPP

#include "rclcpp/rclcpp.hpp"
#include "robot_command.hpp"
#include "mission_file_parser.hpp"

class MissionManager : public rclcpp::Node {
public:
    MissionManager(std::unique_ptr<MissionFileParser> parser);

private:
    void run_missions();

    rclcpp::Publisher<custom_interfaces::msg::Command>::SharedPtr command_publisher;

    std::unique_ptr<MissionFileParser> parser;
    std::vector<std::shared_ptr<Root>> missions;

    bool status_received;
    bool command_successful;
    bool ready;
    bool missions_running;

};

#endif // MISSION_MANAGER_HPP
