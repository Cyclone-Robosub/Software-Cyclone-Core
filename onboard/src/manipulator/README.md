# `manipulator`

## What is it?
The `manipulator` node provides a way for our system to deploy the dropper through ROS2. It listens on a single topic (`manipulator_cmd`) for either a 1 or a 2. If it receives a 1 it deploys dropper 1, and if it receives a 2 it deploys dropper 2. In either case, there is a 2-second period after a deployment in which the deployment thread blocks while waiting to subsequently reset the dropper back to its rest position. It is recommended to wait until a dropper has finished its deployment (2 seconds) before requesting another deployment. It sends its messages over UART to an Arduino Uno, which then adjusts the servo on the dropper to the correct position.

## How do I use it?
Once the node is running, publish a 1 on the `manipulator_cmd` topic (accepts a ROS2 standard message type of unsigned 8-bit integer) to deploy dropper 1 and a 2 to deploy dropper 2. Wait 2 seconds between dropper deployments to allow the dropper to return to its rest state.

The package and node are both called `manipulator`, so you can run the node with `ros2 run manipulator manipulator`.

## What topics/services does the package use for input?
- Topics: `manipulator_cmd`: an unsigned 8-bit integer for which dropper to release
- Services: None

## What topics/services does the package use for output?
- Topics: None
- Services: None

## What custom message types or libraries does the package use?
- Messages: None
- Services: None
- Libraries:
    - `fd_interface`