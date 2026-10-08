#! /bin/bash

source install/setup.bash
echo "Sending go signal in 1 minute"
sleep 30s
echo "Sending go signal in 30 seconds"
sleep 15s
echo "Sending go signal in 15 seconds"
sleep 10s
echo "Sending go signal in 5 seconds"
sleep 5s
ros2 topic pub /go_signal std_msgs/msg/Bool "data: true" --once
echo "Sent go signal"
