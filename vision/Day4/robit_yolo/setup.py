from setuptools import find_packages, setup
import os
from glob import glob


package_name = 'robit_yolo'


setup(
    name=package_name,
    version='0.0.0',

    packages=find_packages(),

    data_files=[
        (
            'share/ament_index/resource_index/packages',
            ['resource/' + package_name]
        ),

        (
            'share/' + package_name,
            ['package.xml']
        ),

        (
            os.path.join(
                'share',
                package_name,
                'launch'
            ),
            glob('launch/*.launch.py')
        ),

        (
            os.path.join(
                'share',
                package_name,
                'config'
            ),
            glob('config/*.yaml')
        ),

        (
            os.path.join(
                'share',
                package_name,
                'models'
            ),
            glob('models/*.pt')
        ),
    ],

    install_requires=[
        'setuptools'
    ],

    zip_safe=True,

    maintainer='changgyu',

    description='ROBIT YOLO26n object detection package',

    license='Apache-2.0',

    entry_points={
        'console_scripts': [
            'camera_node = robit_yolo.camera_node:main',
            'yolo_node = robit_yolo.yolo_node:main',
        ],
    },
)