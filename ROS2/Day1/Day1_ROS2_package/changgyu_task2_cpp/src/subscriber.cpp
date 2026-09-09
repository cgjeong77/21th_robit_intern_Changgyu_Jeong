#include "changgyu_task2_cpp/subscriber.hpp"

#include <functional>

using std::placeholders::_1;

Task2Subscriber::Task2Subscriber()
    : Node("task2_cpp_subscriber")
{
    int_subscriber_ =
        this->create_subscription<std_msgs::msg::Int32>(
            "/task2/int_data",
            10,
            std::bind(&Task2Subscriber::int_callback, this, _1));

    string_subscriber_ =
        this->create_subscription<std_msgs::msg::String>(
            "/task2/string_data",
            10,
            std::bind(&Task2Subscriber::string_callback, this, _1));

    float_subscriber_ =
        this->create_subscription<std_msgs::msg::Float32>(
            "/task2/float_data",
            10,
            std::bind(&Task2Subscriber::float_callback, this, _1));

    bool_subscriber_ =
        this->create_subscription<std_msgs::msg::Bool>(
            "/task2/bool_data",
            10,
            std::bind(&Task2Subscriber::bool_callback, this, _1));
}

void Task2Subscriber::int_callback(
    const std_msgs::msg::Int32::SharedPtr msg)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Received int: %d",
        msg->data);
}

void Task2Subscriber::string_callback(
    const std_msgs::msg::String::SharedPtr msg)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Received string: %s",
        msg->data.c_str());
}

void Task2Subscriber::float_callback(
    const std_msgs::msg::Float32::SharedPtr msg)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Received float: %.2f",
        msg->data);
}

void Task2Subscriber::bool_callback(
    const std_msgs::msg::Bool::SharedPtr msg)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Received bool: %s",
        msg->data ? "true" : "false");
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task2Subscriber>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}