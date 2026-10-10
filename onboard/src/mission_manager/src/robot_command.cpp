#include "robot_command.hpp"

Command::Command(std::unique_ptr<Command> next, std::unique_ptr<Command> failure) :
    next(std::move(next)), failure(std::move(failure)) {
        // intentionally blank
}

DriveToWorldWaypoint::DriveToWorldWaypoint(
        std::unique_ptr<Command> next,
        std::unique_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        std::array<char,16> trick_id = {'\0'}
    ) :
    Command(std::move(next), std::move(failure)),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    hold_time(hold_time),
    trick_id(trick_id) {
        // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> DriveToWorldWaypoint::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->DRIVE_TO_WORLD_WAYPOINT;
    command_msg->position = *goal_waypoint;
    command_msg->position_mask = *waypoint_mask;
    command_msg->tolerance = *tolerance;
    command_msg->hold_time = hold_time;
    command_msg->trick_id = trick_id;
    return command_msg;
}

DriveToWorldWaypointSeeking::DriveToWorldWaypointSeeking(
        std::unique_ptr<Command> next,
        std::unique_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        uint8_t object_id,
        double confidence_required,
        std::array<char,16> trick_id = {'\0'}
    ) :
    Command(std::move(next), std::move(failure)),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    object_id(object_id),
    confidence_required(confidence_required),
    trick_id(trick_id) {
        // intentionally blank
}


std::unique_ptr<custom_interfaces::msg::Command> DriveToWorldWaypointSeeking::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->DRIVE_TO_WORLD_WAYPOINT_SEEKING;
    command_msg->position = *goal_waypoint;
    command_msg->position_mask = *waypoint_mask;
    command_msg->object_id = object_id;
    command_msg->confidence_required = confidence_required;
    command_msg->trick_id = trick_id;
}

