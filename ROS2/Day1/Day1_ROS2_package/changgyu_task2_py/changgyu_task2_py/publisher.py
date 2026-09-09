import rclpy
from rclpy.node import Node

from std_msgs.msg import Int32
from std_msgs.msg import String
from std_msgs.msg import Float32
from std_msgs.msg import Bool


class Task2Publisher(Node):

    def __init__(self):
        super().__init__('task2_py_publisher')

        self.int_publisher = self.create_publisher(
            Int32, '/task2/int_data', 10
        )

        self.string_publisher = self.create_publisher(
            String, '/task2/string_data', 10
        )

        self.float_publisher = self.create_publisher(
            Float32, '/task2/float_data', 10
        )

        self.bool_publisher = self.create_publisher(
            Bool, '/task2/bool_data', 10
        )

        self.timer = self.create_timer(
            1.0, self.publish_task2_msg
        )

        self.count = 0

    def publish_task2_msg(self):
        int_msg = Int32()
        string_msg = String()
        float_msg = Float32()
        bool_msg = Bool()

        int_msg.data = self.count
        string_msg.data = 'Hello ROS2 Python'
        float_msg.data = 3.14
        bool_msg.data = (self.count % 2 == 0)

        self.int_publisher.publish(int_msg)
        self.string_publisher.publish(string_msg)
        self.float_publisher.publish(float_msg)
        self.bool_publisher.publish(bool_msg)

        self.get_logger().info(
            'Publish -> int: {0}, string: {1}, float: {2:.2f}, bool: {3}'.format(
                int_msg.data,
                string_msg.data,
                float_msg.data,
                bool_msg.data
            )
        )

        self.count += 1


def main(args=None):
    rclpy.init(args=args)

    node = Task2Publisher()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        node.get_logger().info('Keyboard Interrupt (SIGINT)')

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()