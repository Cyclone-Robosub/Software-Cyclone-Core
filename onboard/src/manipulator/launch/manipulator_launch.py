from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
   return LaunchDescription([
        Node(
            # name of the package
            package='manipulator',
            # name of the elf
            executable='manipulator',
            # Name of the node
            name="manipulator",
            # Writes to the stdout
            output="screen"
        )
    ])
