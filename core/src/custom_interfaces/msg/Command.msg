# constants for the command type
uint8 IDLE=0
uint8 DRIVE_TO_WORLD_WAYPOINT=1
uint8 DRIVE_TO_WORLD_WAYPOINT_SEEKING=2
uint8 TRACK_OBJECT_WAYPOINT=3
uint8 DURATION_TRICK=4
uint8 DISTANCE_TRICK=5

# constants for object_id type: to be extended by vision team
uint8 OBJECT_A=0
uint8 OBJECT_B=1
# etc.

# the command type, one of IDLE, DRIVE_TO_WORLD_WAYPOINT, etc.
uint8 command

# Used for DRIVE_TO_WORLD_WAYPOINT and DRIVE_TO_WORLD_WAYPOINT_SEEKING and TRACK_OBJECT_WAYPOINT and DISTANCE_TRICK
Pose6D position # for 1 and 2, this is the waypoint. For 3, this is the tracking position. For 5, this is a body-offset waypoint for the trick end.
# Used for DRIVE_TO_WORLD_WAYPOINT and DRIVE_TO_WORLD_WAYPOINT_SEEKING and TRACK_OBJECT_WAYPOINT and DURATION_TRICK and DISTANCE_TRICK
Pose6DMask position_mask

# Used for DRIVE_TO_WORLD_WAYPOINT and TRACK_OBJECT_WAYPOINT and DURATION_TRICK and DISTANCE_TRICK
Tolerance6D tolerance
float64 hold_time

# Used for DRIVE_TO_WORLD_WAYPOINT_SEEKING and TRACK_OBJECT_WAYPOINT
uint8 object_id
float64 confidence_required

# Used for ALL commands except IDLE, primarily for DURATION_TRICK and DISTANCE_TRICK
char[16] trick_id

# Used for DURATION_TRICK
float64 trick_duration
