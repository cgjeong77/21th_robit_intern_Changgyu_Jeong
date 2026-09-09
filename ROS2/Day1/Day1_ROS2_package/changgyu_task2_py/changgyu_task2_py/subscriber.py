import rclpy
from rclpy.node import Node

from std_msgs.msg import Int32
from std_msgs.msg import String
from std_msgs.msg import Float32
from std_msgs.msg import Bool


class Task2Subscriber(Node):

    def __init__(self):
        super().__init__('task2_py_subscriber')

        self.int_subscriber = self.create_subscription(
            Int32,
            '/task2/int_data',
            self.int_callback,
            10
        )

        self.string_subscriber = self.create_subscription(
            String,
            '/task2/string_data',
            self.string_callback,
            10
        )

        self.float_subscriber = self.create_subscription(
            Float32,
            '/task2/float_data',
            self.float_callback,
            10
        )

        self.bool_subscriber = self.create_subscription(
            Bool,
            '/task2/bool_data',
            self.bool_callback,
            10
        )

    def int_callback(self, msg):
        self.get_logger().info(
            'Received int: {0}'.format(msg.data)
        )

    def string_callback(self, msg):
        self.get_logger().info(
            'Received string: {0}'.format(msg.data)
        )

    def float_callback(self, msg):
        self.get_logger().info(
            'Received float: {0:.2f}'.format(msg.data)
        )

    def bool_callback(self, msg):
        self.get_logger().info(
            'Received bool: {0}'.format(msg.data)
        )


def main(args=None):
    rclpy.init(args=args)

    node = Task2Subscriber()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        node.get_logger().info('Keyboard Interrupt (SIGINT)')

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()