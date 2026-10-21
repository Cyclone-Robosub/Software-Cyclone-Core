#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>
#include <chrono>
#include <optional>

#include "custom_interfaces/msg/command.hpp"
#include "custom_interfaces/msg/pose6_d.hpp"
#include "custom_interfaces/msg/pose6_d_mask.hpp"
#include "custom_interfaces/msg/tolerance6_d.hpp"

#include "waypoint.hpp"

// TODO: add name (not sent to ROS)

/* Abstract base class representing a generic Command executed in the course of a mission */
class Command {
public:
    Command(std::string name);
    Command(std::string name, double timeout);
    Command(std::string name, std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double timeout = -1);

    virtual std::unique_ptr<custom_interfaces::msg::Command> start_command_and_get_message();
    virtual std::shared_ptr<Command> get_next();
    virtual std::shared_ptr<Command> get_failure();
    virtual std::string get_name();
    virtual void set_next(std::shared_ptr<Command> command);
    virtual void set_failure(std::shared_ptr<Command> command);
    virtual bool mission_complete();
    virtual bool timeout_exceeded();
    virtual double get_seconds_remaining();
    virtual double get_initial_duration_seconds();
    virtual void custom_start_action();
    virtual void custom_finish_action();
    virtual bool custom_success_criteria_met();

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() = 0;

    std::string name;
    std::shared_ptr<Command> next;
    std::shared_ptr<Command> failure;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    double timeout;
};

class DriveToWorldWaypoint : public Command {
public:
    DriveToWorldWaypoint(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    );
    DriveToWorldWaypoint(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    );

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
    std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    double hold_time;
    std::array<uint8_t,16> trick_id;
};

class DriveToWorldWaypointSeeking : public Command {
public:
    DriveToWorldWaypointSeeking(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    );
    DriveToWorldWaypointSeeking(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::array<uint8_t,16> object_id,
        double confidence_required,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    );

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
    std::unique_ptr<custom_interfaces::msg::Pose6D> goal_waypoint;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::array<uint8_t,16> object_id;
    double confidence_required;
    std::array<uint8_t,16> trick_id;
};

class Idle : public Command {
public:
    /* Idle indefinitely */
    Idle(std::string name, double duration);
    Idle(std::string name, std::shared_ptr<Command> next, std::shared_ptr<Command> failure, double duration);

    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
    virtual bool custom_success_criteria_met() override;
    virtual double get_seconds_remaining() override;
    virtual double get_initial_duration_seconds() override;
protected:
    double duration;
};

class TrackObjectWaypoint : public Command {
public:
    TrackObjectWaypoint(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> tracking_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> object_id,
        double hold_time,
        double timeout,
        std::optional<std::array<uint8_t,16>> trick_id
    );
    TrackObjectWaypoint(
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
    );

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
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
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
    );
    DurationTrick(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double duration,
        double timeout
    );

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask;
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance;
    std::array<uint8_t,16> trick_id;
    double hold_time;
    double duration;
};

class DistanceTrick: public Command {
public:
    DistanceTrick(
        std::string name,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    );
    DistanceTrick(
        std::string name,
        std::shared_ptr<Command> next,
        std::shared_ptr<Command> failure,
        std::unique_ptr<custom_interfaces::msg::Pose6D> destination_position,
        std::unique_ptr<custom_interfaces::msg::Pose6DMask> waypoint_mask,
        std::unique_ptr<custom_interfaces::msg::Tolerance6D> tolerance,
        std::array<uint8_t,16> trick_id,
        double hold_time,
        double timeout
    );

protected:
    virtual std::unique_ptr<custom_interfaces::msg::Command> get_ros2_message() override;
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
