#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include <rclcpp/rclcpp.hpp>
#include <changgyu_day2_interfaces/msg/add_two_ints.hpp>

class Task1Subscriber : public rclcpp::Node
{
public:
    Task1Subscriber();

private:
    rclcpp::Subscription<
        changgyu_day2_interfaces::msg::AddTwoInts>::SharedPtr subscription_;

    void callback(
        const changgyu_day2_interfaces::msg::AddTwoInts::SharedPtr msg);
};

#endif