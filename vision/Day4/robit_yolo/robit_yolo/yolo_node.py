import os
import time
import threading
import cv2
import torch
import rclpy

from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from ultralytics import YOLO
from ament_index_python.packages import get_package_share_directory


class YoloNode(Node):
    def __init__(self):
        super().__init__('yolo_node')

        self.declare_parameter('input_topic', '/camera/image_raw')
        self.declare_parameter('hz', 8.0)
        self.declare_parameter('model', 'yolo26n.pt')
        self.declare_parameter('confidence', 0.3)
        self.declare_parameter('imgsz', 320)
        self.declare_parameter('class_ids', [0])
        self.declare_parameter('class_names', ['person'])

        topic = self.get_parameter('input_topic').value
        hz = self.get_parameter('hz').value
        model = self.get_parameter('model').value
        self.conf = self.get_parameter('confidence').value
        self.imgsz = self.get_parameter('imgsz').value
        self.ids = self.get_parameter('class_ids').value
        names = self.get_parameter('class_names').value

        self.names = dict(zip(self.ids, names))

        path = os.path.join(
            get_package_share_directory('robit_yolo'),
            'models',
            model
        )

        torch.set_num_threads(2)
        cv2.setNumThreads(1)

        self.model = YOLO(path)
        self.bridge = CvBridge()

        self.frame = None
        self.boxes = []
        self.busy = False

        qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )

        cv2.namedWindow('YOLO26n Detection', cv2.WINDOW_NORMAL)
        cv2.resizeWindow('YOLO26n Detection', 1280, 720)

        self.create_subscription(
            Image,
            topic,
            self.image_callback,
            qos
        )

        self.create_timer(
            1.0 / hz,
            self.timer_callback
        )

    def image_callback(self, msg):
        frame = self.bridge.imgmsg_to_cv2(msg, 'bgr8')
        self.frame = frame.copy()

        for x1, y1, x2, y2, name, conf in self.boxes:
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(
                frame,
                f'{name} {conf:.2f}',
                (x1, max(y1 - 10, 20)),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 255, 0),
                2
            )

        cv2.imshow('YOLO26n Detection', frame)
        cv2.waitKey(1)

    def timer_callback(self):
        if self.frame is None or self.busy:
            return

        self.busy = True

        threading.Thread(
            target=self.detect,
            args=(self.frame.copy(),),
            daemon=True
        ).start()

    def detect(self, frame):
        start = time.perf_counter()

        result = self.model(
            frame,
            device='cpu',
            imgsz=self.imgsz,
            conf=self.conf,
            classes=self.ids,
            verbose=False
        )[0]

        latency = (time.perf_counter() - start) * 1000
        self.get_logger().info(f'Inference latency: {latency:.2f} ms')

        boxes = []

        for box in result.boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            class_id = int(box.cls[0])
            conf = float(box.conf[0])

            boxes.append((
                x1, y1, x2, y2,
                self.names[class_id],
                conf
            ))

        self.boxes = boxes
        self.busy = False

    def destroy_node(self):
        cv2.destroyAllWindows()
        super().destroy_node()


def main():
    rclpy.init()
    node = YoloNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()