# `thrust_interface`

## What is it?
This ROS node communicates with the Pi Pico over serial, sending PWM signals that it has received from `soft_mux`. It expects to find the Pico in `/dev/serial/by-id/PICO_ID`. If you replace the Pico, you'll need to update the this file path to the correct Pico ID in the `main()` method in `thrust_interface.cpp`.

## How do I use it?
To communicate with this node, publish PWMs that you want to send to the Pi Pico on the `pwm_cmd` topic, and publish a heartbeat on the `mux_heartbeat` topic. This heartbeat must be published at least once per second, or the node will send a stop command (all 1500 PWMs) to the Pico. It's recommended to publish the heartbeat every half a second to prevent unintentional stop commands. `thrust_interface` itself doesn't publish to any topics.

## What nodes/topics/services does depend on?
- Nodes: none in particular, but will need some node to publish to the topics it's subscribed to (below)
- Topics:
    - `pwm_cmd`: contains the list of 8 PWM signals that should be sent to the robot thrusters
    - `mux_heartbeat`: used to determine if we have an established connection to `soft_mux`.
- Services: none