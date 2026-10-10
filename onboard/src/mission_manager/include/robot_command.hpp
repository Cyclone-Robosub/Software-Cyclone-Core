#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>

#include "rclcpp.hpp"
#include "custom_interfaces/msg/command.hpp"
#include "custom_interfaces/msg/pose6_d.hpp"
#include "custom_interfaces/msg/pose6_d_mask.hpp"
#include "custom_interfaces/msg/tolerance6_d.hpp"

#include "waypoint.hpp"

/* Abstract base class representing a generic Command executed in the course of a mission */
class Command {
public:
    Command(std::shared_ptr<Command> next, std::shared_ptr<Command> failure);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() = 0;
    virtual std::shared_ptr<Command> get_next();
    virtual std::shared_ptr<Command> get_failure();

protected:
    std::shared_ptr<Command> next;
    std::shared_ptr<Command> failure;
};

class DriveToWorldWaypoint : public Command {
public:
    DriveToWorldWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        std::array<uint8_t,16> trick_id = {'\0'}
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    double hold_time;
    std::array<uint8_t,16> trick_id;
};

class DriveToWorldWaypointSeeking : public Command {
public:
    DriveToWorldWaypointSeeking(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        uint8_t object_id,
        double confidence_required,
        std::array<uint8_t,16> trick_id = {'\0'}
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    uint8_t object_id;
    double confidence_required;
    std::array<uint8_t,16> trick_id;
};

class Idle : public Command {
public:
    Idle(std::shared_ptr<Command> next, std::shared_ptr<Command> failure);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
};

class TrackObjectWaypoint : public Command {
public:
    TrackObjectWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        uint8_t object_id,
        double hold_time,
        std::array<uint8_t,16> trick_id = {'\0'}
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    uint8_t object_id;
    double hold_time;
    std::array<uint8_t,16> trick_id;
};

class DurationTrick : public Command {
public:
    DurationTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    std::array<uint8_t,16> trick_id;
    double hold_time;
    double duration;
};

class DistanceTrick: public Command {
public:
    DistanceTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    std::array<uint8_t,16> trick_id;
    double hold_time;
};

#endif // COMMAND_HPP
