#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>
#include <chrono>

#include "custom_interfaces/msg/command.hpp"
#include "custom_interfaces/msg/pose6_d.hpp"
#include "custom_interfaces/msg/pose6_d_mask.hpp"
#include "custom_interfaces/msg/tolerance6_d.hpp"

#include "waypoint.hpp"

// TODO: add name (not sent to ROS)

/* Abstract base class representing a generic Command executed in the course of a mission */
class Command {
public:
    Command();
    Command(double timeout);
    Command(std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double timeout = -1);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() = 0;
    virtual std::shared_ptr<Command> get_next();
    virtual std::shared_ptr<Command> get_failure();
    virtual void set_next(std::shared_ptr<Command> command);
    virtual void set_failure(std::shared_ptr<Command> command);
    virtual bool mission_complete();
    virtual bool timeout_exceeded(std::chrono::time_point<std::chrono::steady_clock> start_time);

protected:
    std::shared_ptr<Command> next;
    std::shared_ptr<Command> failure;
    std::string name; // TODO: add to constructors
    double timeout; // TODO: add to constructors
};

class DriveToWorldWaypoint : public Command {
public:
    DriveToWorldWaypoint(
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
        std::array<uint8_t,16> trick_id = {'\0'}
    );
    DriveToWorldWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
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
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::array<uint8_t,16> trick_id = {'\0'}
    );
    DriveToWorldWaypointSeeking(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::array<uint8_t,16> trick_id = {'\0'}
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::array<uint8_t,16> object_id;
    double confidence_required;
    std::array<uint8_t,16> trick_id;
};

class Idle : public Command {
public:
    /* Idle indefinitely */
    Idle();
    Idle(double duration);
    Idle(std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double duration);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
protected:
    double duration;
};

class TrackObjectWaypoint : public Command {
public:
    TrackObjectWaypoint(
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        double timeout,
        std::array<uint8_t,16> trick_id = {'\0'}
    );
    TrackObjectWaypoint(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        double timeout,
        std::array<uint8_t,16> trick_id = {'\0'}
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    std::array<uint8_t,16> object_id;
    double hold_time;
    std::array<uint8_t,16> trick_id;
};

class DurationTrick : public Command {
public:
    DurationTrick(
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
    );
    DurationTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
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
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    );
    DistanceTrick(
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    );

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;

protected:
    std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    std::array<uint8_t,16> trick_id;
    double hold_time;
};

/* Special Commands */

/* The start of every decision tree. Can be treated as an Idle if desired. */
class Root: public Command {
public:
    Root();
    Root(std::shared_ptr<Command> next);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
};

/* Included as the final node reached in any decision tree. Can be treated as an Idle if desired. */
class Finish : public Command {
public:
    Finish();

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
    virtual std::shared_ptr<Command> get_next() override;
    virtual std::shared_ptr<Command> get_failure() override;
    virtual bool mission_complete() override;
};

#endif // COMMAND_HPP
