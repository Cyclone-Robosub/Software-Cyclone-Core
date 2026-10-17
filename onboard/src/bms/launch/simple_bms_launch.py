from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
   return LaunchDescription([
        Node(
            # name of the package
            package='simple_bms',
            # name of the elf
            executable='simple_bms',
            # Name of the node
            name="simple_bms",
            # Writes to the stdout
            output="screen"
        )
    ])
