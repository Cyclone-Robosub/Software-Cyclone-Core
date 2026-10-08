from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
   return LaunchDescription([
        Node(
            # name of the package
            package='data_logger',
            # name of the elf
            executable='data_logger',
            # Name of the node
            name="data_logger",
            # Writes to the stdout
            output="screen"
        )
    ])
