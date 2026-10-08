#!/bin/bash

if [[ ! -f /opt/ros/jazzy/setup.bash ]]; then
	echo "ROS not found. Attempting to re-run in distrobox container..."
	distrobox enter ubuntu-ros -- $0 # Run this script, now in container
	distrobox enter ubuntu-ros # After finished, stay in the container
	exit # Don't run the script again after we're done
fi

source install/setup.sh
source ~/.$(echo $SHELL | awk -F  '/' '{print $NF}')rc # make sure we have PATH up to date

SESSION="laptop"

# --- TUI ---
# Create new detached tmux session
TUI_PANE=$(tmux new-session -d -s $SESSION -n 'tui' -P -F "#{pane_id}" "ros2 run tui tui; bash" )

# --- Rosbridge ---
# Create new window for joystick systems
JOYSTICK_PANE=$(tmux new-window -t $SESSION -n 'joystick' -P -F "#{pane_id}" "ros2 launch rosbridge_server rosbridge_websocket_launch.xml; bash" )

# --- Webpage ---
# Sleep so that the rosbridge loads first
WEBPAGE_PANE=$(tmux split-window -h -t $JOYSTICK_PANE -P -F "#{pane_id}" "sleep 2; distrobox-host-exec firefox remote_control_webpage/index.html; bash" )

# --- Simple Joystick ---
# ========= UNCOMMENT IF DESIRED =========
# tmux split-window -v -t $WEBPAGE_PANE "ros2 run simple_joystick_controller Simple_Joystick_Controller; bash"

# Joystick telemetry
# Sleep so that the webpage has loaded and the topic is being broadcast
tmux split-window -v -t $JOYSTICK_PANE "sleep 2.5; ros2 topic echo /ps5_controller; bash"


# Select the main window
tmux select-window -t $SESSION:0

# Bring the configured tmux session to the foreground
tmux attach -t $SESSION
