#include "changgyu_task2_cpp/publisher.hpp"

#include <chrono>
#include <functional>

using namespace std::chrono_literals;

Task2Publisher::Task2Publisher()
    : Node("task2_cpp_publisher"), count_(0)
{
    int_publisher_ =
        this->create_publisher<std_msgs::msg::Int32>(
            "/task2/int_data", 10);

    string_publisher_ =
        this->create_publisher<std_msgs::msg::String>(
            "/task2/string_data", 10);

    float_publisher_ =
        this->create_publisher<std_msgs::msg::Float32>(
            "/task2/float_data", 10);

    bool_publisher_ =
        this->create_publisher<std_msgs::msg::Bool>(
            "/task2/bool_data", 10);

    timer_ = this->create_wall_timer(
        1s,
        std::bind(&Task2Publisher::timer_callback, this));
}

void Task2Publisher::timer_callback()
{
    auto int_msg = std_msgs::msg::Int32();
    auto string_msg = std_msgs::msg::String();
    auto float_msg = std_msgs::msg::Float32();
    auto bool_msg = std_msgs::msg::Bool();

    int_msg.data = count_;
    string_msg.data = "Hello ROS2 C++";
    float_msg.data = 3.14f;
    bool_msg.data = (count_ % 2 == 0);

    int_publisher_->publish(int_msg);
    string_publisher_->publish(string_msg);
    float_publisher_->publish(float_msg);
    bool_publisher_->publish(bool_msg);

    RCLCPP_INFO(
        this->get_logger(),
        "Publish -> int: %d, string: %s, float: %.2f, bool: %s",
        int_msg.data,
        string_msg.data.c_str(),
        float_msg.data,
        bool_msg.data ? "true" : "false");

    count_++;
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task2Publisher>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}