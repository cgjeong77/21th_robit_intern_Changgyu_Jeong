import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    package_share = get_package_share_directory('robit_yolo')

    config_file = os.path.join(
        package_share,
        'config',
        'yolo.yaml'
    )

    camera_node = Node(
        package='robit_yolo',
        executable='camera_node',
        name='camera_node',
        output='screen',
        parameters=[config_file]
    )

    yolo_node = Node(
        package='robit_yolo',
        executable='yolo_node',
        name='yolo_node',
        output='screen',
        parameters=[config_file]
    )

    return LaunchDescription([
        camera_node,
        yolo_node
    ])