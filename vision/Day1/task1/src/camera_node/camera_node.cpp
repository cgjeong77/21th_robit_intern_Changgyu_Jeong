#include "camera_ui_pkg/camera_node/camera_node.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <stdexcept>

CameraNode::CameraNode()
: Node("camera_node")
{
    declare_parameter<std::string>("topic", "/camera/image");
    declare_parameter<double>("hz", 30.0);
    declare_parameter<int>("camera_index", 0);

    topic_ = get_parameter("topic").as_string();
    hz_ = get_parameter("hz").as_double();
    camera_index_ = get_parameter("camera_index").as_int();

    if (hz_ <= 0.0) {
        hz_ = 30.0;
    }

    image_pub_ = create_publisher<sensor_msgs::msg::Image>(topic_, 10);

    cap_.open(camera_index_);

    if (!cap_.isOpened()) {
        RCLCPP_ERROR(get_logger(), "Camera open failed");
        throw std::runtime_error("Camera open failed");
    }

    auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(1.0 / hz_)
    );

    timer_ = create_wall_timer(
        period,
        std::bind(&CameraNode::timerCallback, this)
    );

    RCLCPP_INFO(get_logger(), "Camera node started");
    RCLCPP_INFO(get_logger(), "Topic: %s", topic_.c_str());
    RCLCPP_INFO(get_logger(), "Hz: %.1f", hz_);
    RCLCPP_INFO(get_logger(), "Camera index: %d", camera_index_);
}

void CameraNode::timerCallback()
{
    cv::Mat frame;

    cap_ >> frame;

    if (frame.empty()) {
        RCLCPP_WARN(get_logger(), "Camera frame is empty");
        return;
    }

    std_msgs::msg::Header header;
    header.stamp = now();
    header.frame_id = "camera";

    auto msg = cv_bridge::CvImage(header, "bgr8", frame).toImageMsg();

    image_pub_->publish(*msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<CameraNode>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}