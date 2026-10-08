#include "command.hpp"

Command::Command(std::unique_ptr<Command> next, std::unique_ptr<Command> failure) :
    next(std::move(next)), failure(std::move(failure)) {
        // intentionally blank
}

DriveToWorldWaypoint::DriveToWorldWaypoint(std::unique_ptr<Command> next, std::unique_ptr<Command> failure,
        Waypoint6D goal_waypoint, WaypointMask6D waypoint_mask, Waypoint6D tolerance, double hold_time) :
    Command(std::move(next), std::move(failure)), goal_waypoint(goal_waypoint), waypoint_mask(waypoint_mask),
    tolerance(tolerance), hold_time(hold_time) {
        // intentionally blank
    }

DriveToWorldWaypoint::execute() {
    
}