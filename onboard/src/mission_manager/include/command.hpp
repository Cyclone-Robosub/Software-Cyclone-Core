#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>

#include "rclcpp.hpp"

#include "waypoint.hpp"

/* Abstract base class representing a generic Command executed in the course of a mission */
class Command {
public:
    Command(std::unique_ptr<Command> next, std::unique_ptr<Command> failure);

    virtual void execute() = 0;

protected:
    std::unique_ptr<Command> next;
    std::unique_ptr<Command> failure;

};

class DriveToWorldWaypoint : public Command {
public:
    DriveToWorldWaypoint(std::unique_ptr<Command> next, std::unique_ptr<Command> failure,
        Waypoint6D goal_waypoint, WaypointMask6D waypoint_mask, Waypoint6D tolerance, double hold_time);

    virtual void execute() override;

protected:
   Waypoint6D goal_waypoint;
   WaypointMask6D waypoint_mask;
   Waypoint6D tolerance;
   double hold_time; 
};

#endif // COMMAND_HPP
