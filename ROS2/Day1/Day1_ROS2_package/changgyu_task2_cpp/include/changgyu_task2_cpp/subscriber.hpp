#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include <rclcpp/rclcpp.hpp>

#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/bool.hpp>

class Task2Subscriber : public rclcpp::Node
{
public:
    Task2Subscriber();

private:
    void int_callback(const std_msgs::msg::Int32::SharedPtr msg);
    void string_callback(const std_msgs::msg::String::SharedPtr msg);
    void float_callback(const std_msgs::msg::Float32::SharedPtr msg);
    void bool_callback(const std_msgs::msg::Bool::SharedPtr msg);

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr int_subscriber_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr string_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr float_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr bool_subscriber_;
};

#endif