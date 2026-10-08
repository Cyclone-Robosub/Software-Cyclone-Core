# PS5 Gamepad to control the submarine

# Type name: `Gamepad`
| Name  | Type      | Value range | Notes                 |
| ----- | --------- | ----------- | --------------------- |
| x     | `float32` | [-1.0, 1.0] | Left joystick x axis  |
| y     | `float32` | [-1.0, 1.0] | Left joystick y axis  |
| rise  | `float32` | [0.0, 1.0]  | Activates rise (RT)   |
| sink  | `float32` | [0.0, 1.0]  | Activates sink (LT)   |
| yaw   | `float32` | [-1.0, 1.0] | Right joystick x axis |
| pitch | `float32` | [-1.0, 1.0] | Right joystick y axis |

## How to run the web controller
0. `source install/setup.bash`
1. Run the rosbridge server first. This node bridge ros to the browser. 
```
ros2 launch rosbridge_server rosbridge_websocket_launch.xml
```
2. Run `ros2 run joystick_logger logger` to advertise the Gamepad topic.
