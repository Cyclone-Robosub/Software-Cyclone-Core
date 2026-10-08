#ifndef WAYPOINT_HPP
#define WAYPOINT_HPP

typedef struct {
    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;
} Waypoint6D;

typedef struct {
    bool x;
    bool y;
    bool z;
    bool roll;
    bool pitch;
    bool yaw;
} WaypointMask6D;

#endif // WAYPOINT_HPP
