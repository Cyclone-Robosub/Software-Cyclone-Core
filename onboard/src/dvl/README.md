# `dvl`

## What is it?
This package contains the implementation of the dvl node in Ros2.

## How do I use it?
The package is called `dvl` and the node is called `dvl`, so to run it type `ros2 run dvl dvl`.

The velocity report is published to `velocity_report` topic, the dead reckoning report is published to `dead_reck_report` topic, and when the `set_config` service is called, the configuration is published to `config` topic.

The node can be set/reset through the `set_config`, `set_drr`, and `set_gyro` services. 

To force the dvl to trigger a ping with its acoustic sensors, use the `triggerPing` service.

## What nodes/topics/services does depend on?
None