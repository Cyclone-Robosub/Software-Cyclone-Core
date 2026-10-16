#include "robot_command.hpp"

Command::Command() : next(std::make_shared<Finish>()), failure(std::make_shared<Finish>()) {
    // intentionally blank
}

Command::Command(std::shared_ptr<Command> next, std::shared_ptr<Command> failure) :
    next(std::move(next)), failure(std::move(failure)) {
        // intentionally blank
}

std::shared_ptr<Command> Command::get_next() {
    return next;
}

std::shared_ptr<Command> Command::get_failure() {
    return failure;
}

void Command::set_next(std::shared_ptr<Command> command) {
    next = command;
}
void Command::set_failure(std::shared_ptr<Command> command) {
    failure = command;
}

bool Command::mission_complete() {
    return false;
}

DriveToWorldWaypoint::DriveToWorldWaypoint() : Command() {
    // intentionally blank
};

DriveToWorldWaypoint::DriveToWorldWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        std::array<uint8_t,16> trick_id
    ) :
    Command(next, failure),
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

DriveToWorldWaypointSeeking::DriveToWorldWaypointSeeking() : Command() {
    // intentionally blank
}

DriveToWorldWaypointSeeking::DriveToWorldWaypointSeeking(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        std::array<uint8_t,16> trick_id
    ) :
    Command(next, failure),
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
    return command_msg;
}

Idle::Idle(std::shared_ptr<Command> next, std::shared_ptr<Command> failure) :
    Command(next, failure) {
        // intentionally blank
}

Idle::Idle() : Command() {
    // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> Idle::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->IDLE;
    return command_msg;
}

TrackObjectWaypoint::TrackObjectWaypoint() : Command() {
    // intentionally blank
}

TrackObjectWaypoint::TrackObjectWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        std::array<uint8_t,16> trick_id
    ) :
    Command(next, failure),
    tracking_position(std::move(tracking_position)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    object_id(object_id),
    hold_time(hold_time),
    trick_id(trick_id) {
        // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> TrackObjectWaypoint::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->TRACK_OBJECT_WAYPOINT;
    command_msg->position = *tracking_position;
    command_msg->position_mask = *waypoint_mask;
    command_msg->object_id = object_id;
    command_msg->tolerance = *tolerance;
    command_msg->hold_time = hold_time;
    command_msg->trick_id = trick_id;
    return command_msg;
}

DurationTrick::DurationTrick() : Command() {
    // intentionally blank
}

DurationTrick::DurationTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration
    ) :
    Command(next, failure),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    trick_id(trick_id),
    hold_time(hold_time),
    duration(duration) {
        // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> DurationTrick::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->DURATION_TRICK;
    command_msg->position_mask = *waypoint_mask;
    command_msg->tolerance = *tolerance;
    command_msg->trick_id = trick_id;
    command_msg->hold_time = hold_time;
    command_msg->trick_duration = duration;
    return command_msg;
}

DistanceTrick::DistanceTrick() : Command() {
    // intentionally blank
}

DistanceTrick::DistanceTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time
    ) :
    Command(next, failure),
    destination_position(std::move(destination_position)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    trick_id(trick_id),
    hold_time(hold_time) {
        // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> DistanceTrick::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->DISTANCE_TRICK;
    command_msg->position = *destination_position;
    command_msg->position_mask = *waypoint_mask;
    command_msg->tolerance = *tolerance;
    command_msg->trick_id = trick_id;
    command_msg->hold_time = hold_time;
    return command_msg;
}

Root::Root() : Command() {
    // intentionally blank
}

Root::Root(std::shared_ptr<Command> next) : Command(next, std::make_shared<Root>()) {
    // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> Root::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->IDLE;
    return command_msg;
}

Finish::Finish() : Command(nullptr, nullptr) {
    // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> Finish::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->IDLE;
    return command_msg;
}

std::shared_ptr<Command> Finish::get_next() {
    return std::make_shared<Finish>();
}

std::shared_ptr<Command> Finish::get_failure() {
    return std::make_shared<Finish>();
}

bool Finish::mission_complete() {
    return true;
}
