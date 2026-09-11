from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    yaml_file = os.path.join(
        get_package_share_directory("changgyu_day3_task1_cpp"),
        "config",
        "task1.yaml"
    )

    return LaunchDescription([

        Node(
            package="turtlesim",
            executable="turtlesim_node",
            name="turtlesim"
        ),

        Node(
            package="changgyu_day3_task1_cpp",
            executable="task1_node",
            name="task1_controller",
            parameters=[yaml_file],
            prefix="gnome-terminal --wait --"
        )
    ])