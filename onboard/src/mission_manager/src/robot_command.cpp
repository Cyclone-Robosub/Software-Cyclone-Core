#include "robot_command.hpp"

using namespace std::chrono_literals;

Command::Command(std::string name) :
    name(name),
    next(std::make_shared<Finish>()),
    failure(std::make_shared<Finish>()),
    timeout(-1) {
    // intentionally blank
}

Command::Command(std::string name, double timeout) :
    name(name),
    next(std::make_shared<Finish>()),
    failure(std::make_shared<Finish>()),
    timeout(timeout) {
    // intentionally blank
}

Command::Command(std::string name, std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double timeout) :
    name(name),
    next(std::move(next)),
    failure(std::move(failure)),
    timeout(timeout) {
        // intentionally blank
}


std::unique_ptr<custom_interfaces::msg::Command> Command::start_command_and_get_message() {
    start_time = std::chrono::steady_clock::now();
    return get_ros2_message();
}

std::shared_ptr<Command> Command::get_next() {
    return next;
}

std::shared_ptr<Command> Command::get_failure() {
    return failure;
}

std::string Command::get_name() {
    return name;
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

double Command::get_seconds_remaining() {
    if (timeout == -1) {
        return -1;
    }
    std::chrono::time_point now = std::chrono::steady_clock::now();
    auto ms_remaining = (start_time + (timeout * 1s)) - now;
    return ms_remaining / 1s;
}

double Command::get_initial_duration_seconds() {
    return timeout;
}

bool Command::timeout_exceeded() {
    if (timeout < 0) {
        return false;
    }
    std::chrono::time_point now = std::chrono::steady_clock::now();
    return now > (start_time + (timeout * 1000 * 1ms));
}

void Command::custom_start_action() {
    /* nothing to do: some commands might have something though, like activating the dropper. */
    return;
}

void Command::custom_finish_action() {
    /* nothing to do: some commands might have something though, like activating the dropper. */
    return;
}

bool Command::custom_success_criteria_met() {
    /* nothing special for base Command, but some might have extra. */
    return false;
}

DriveToWorldWaypoint::DriveToWorldWaypoint(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, timeout),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    hold_time(hold_time),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'}))
    {
        // intentionally blank
};

DriveToWorldWaypoint::DriveToWorldWaypoint(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, next, failure, timeout),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    hold_time(hold_time),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'})) {
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
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, timeout),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    object_id(object_id),
    confidence_required(confidence_required),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'})) {
        // intentionally blank
}

DriveToWorldWaypointSeeking::DriveToWorldWaypointSeeking(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, next, failure, timeout),
    goal_waypoint(std::move(goal_waypoint)),
    waypoint_mask(std::move(waypoint_mask)),
    object_id(object_id),
    confidence_required(confidence_required),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'})) {
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

Idle::Idle(std::string name, std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double duration) :
    Command(name, next, failure), duration(duration) {
        // intentionally blank
}

Idle::Idle(std::string name, double duration) : Command(name), duration(duration) {
    // intentionally blank
}

double Idle::get_seconds_remaining() {
    if (duration == -1) {
        return -1;
    }
    std::chrono::time_point now = std::chrono::steady_clock::now();
    auto ms_remaining = (start_time + (duration * 1s)) - now;
    return ms_remaining / 1s;
}

double Idle::get_initial_duration_seconds() {
    return duration;
}

std::unique_ptr<custom_interfaces::msg::Command> Idle::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->IDLE;
    return command_msg;
}

bool Idle::custom_success_criteria_met() {
    std::chrono::time_point now = std::chrono::steady_clock::now();
    return now > (start_time + (duration * 1000 * 1ms));
}

TrackObjectWaypoint::TrackObjectWaypoint(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, timeout),
    tracking_position(std::move(tracking_position)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    object_id(object_id),
    hold_time(hold_time),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'})) {
        // intentionally blank
}

TrackObjectWaypoint::TrackObjectWaypoint(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    ) :
    Command(name, next, failure, timeout),
    tracking_position(std::move(tracking_position)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    object_id(object_id),
    hold_time(hold_time),
    trick_id(trick_id.value_or(std::array<uint8_t,16>{'0'})) {
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

DurationTrick::DurationTrick(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
    ) :
    Command(name, timeout),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    trick_id(trick_id),
    hold_time(hold_time),
    duration(duration) {
        // intentionally blank
}

DurationTrick::DurationTrick(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
    ) :
    Command(name, next, failure, timeout),
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

DistanceTrick::DistanceTrick(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    ) :
    Command(name, timeout),
    destination_position(std::move(destination_position)),
    waypoint_mask(std::move(waypoint_mask)),
    tolerance(std::move(tolerance)),
    trick_id(trick_id),
    hold_time(hold_time) {
        // intentionally blank
}

DistanceTrick::DistanceTrick(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    ) :
    Command(name, next, failure, timeout),
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

Root::Root() : Command("root") {
    // intentionally blank
}

Root::Root(std::shared_ptr<Command> next) : Command("root", next, std::make_shared<Root>()) {
    // intentionally blank
}

std::unique_ptr<custom_interfaces::msg::Command> Root::get_ros2_message() {
    auto command_msg = std::make_unique<custom_interfaces::msg::Command>();
    command_msg->command = command_msg->IDLE;
    return command_msg;
}

Finish::Finish() : Command("finish", nullptr, nullptr) {
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
