#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include <rclcpp/rclcpp.hpp>

#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/bool.hpp>

class Task2Publisher : public rclcpp::Node
{
public:
    Task2Publisher();

private:
    void timer_callback();

    rclcpp::TimerBase::SharedPtr timer_;

    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr int_publisher_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr string_publisher_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr float_publisher_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr bool_publisher_;

    int count_;
};

#endif