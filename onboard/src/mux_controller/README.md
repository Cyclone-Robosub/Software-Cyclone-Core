# `mux_controller`

## What is it?
This is a small terminal program to control the `soft_mux` node (i.e. to switch between listening to the CLI or listening to Matlab). It monitors the current mux mode and updates when it changes, whether the change was initiated by `mux_controller` or a different node.

## How do I use it?
Run it in the terminal, and type either `0` or `1` to select which input `soft_mux` listens to. If you get a message saying `No heartbeat detected from Mux!`, `soft_mux` probably isn't running or isn't able to connect to this program. Check that you're on the same network as `soft_mux`.

The package and node are both called `mux_controller`, so to run it type `ros2 run mux_controller mux_controller`.

## What nodes/topics/services does depend on?
- Node: `softmux`
- Topics:
    - `current_mode`: used to determine what mode `soft_mux` is currently in.
    - `mux_heartbeat`: used to determine if we have an established connection to `soft_mux`.
- Services:
    - `control_mode`: used to modify (set) the current control mode on `soft_mux`.
    - `force_pub`: used when `mux_controller` needs to request the current control mode during startup (`soft_mux` otherwise only publishes when its mode changes).

## What's up with the crazy input parsing stuff going on in the code?
In order to handle screen refreshes erasing typed input, we want to maintain our own input buffer that we can discard whenever the screen is refreshed. In order to do this, we have to read in text *before* the user hits \<Enter\>. From what I can tell, this can only really be done in non-canonical input mode, which also has the effect of treating control characters (like backspace, arrow keys, etc.) as regular input characters, and therefore creating a very unintuitive and annoying user experience. To mitigate this, I have implemented my own rudimentary input parser that handles common control characters as expected. If you have a better, more elegant solution, feel free to improve it! I know it's pretty intense.