import os
from pathlib import Path
    
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource

def generate_launch_description() -> LaunchDescription:
    """
    Based on Duke Robotics
        Generate a ROS 2 launch description that launches all nodes needed for the robot to function, except task planning.
        (Launches only soft_mux and thrust_interface XML files)

        Returns:
            LaunchDescription: The launch description containing the included launch files.
    """
    pkg_soft_mux = Path(get_package_share_directory('soft_mux'))
    pkg_thrust_interface = Path(get_package_share_directory('thrust_interface'))
   
    robot_name = os.getenv('ROBOT_NAME')

    ld = LaunchDescription()

    ld.add_action(IncludeLaunchDescription(
        XMLLaunchDescriptionSource(str(pkg_soft_mux / 'launch' / 'soft_mux_launch.xml')),
    ))

    ld.add_action(IncludeLaunchDescription(
        XMLLaunchDescriptionSource(str(pkg_thrust_interface / 'launch' / 'thrust_interface_launch.xml')),
    ))

    return ld