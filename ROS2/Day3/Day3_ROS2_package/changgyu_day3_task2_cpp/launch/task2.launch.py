from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    yaml_file = os.path.join(
        get_package_share_directory("changgyu_day3_task2_cpp"),
        "config",
        "task2.yaml"
    )

    return LaunchDescription([

        Node(
            package="changgyu_day3_task2_cpp",
            executable="traffic_light_node",
            name="traffic_light_node",
            parameters=[yaml_file],
            output="screen"
        ),

        Node(
            package="changgyu_day3_task2_cpp",
            executable="vehicle_node",
            name="vehicle_node",
            parameters=[yaml_file],
            output="screen"
        ),

        Node(
            package="changgyu_day3_task2_cpp",
            executable="traffic_gui",
            name="traffic_gui",
            output="screen"
        )

    ])