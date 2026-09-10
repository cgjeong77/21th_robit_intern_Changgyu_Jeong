#ifndef TASK2_HPP
#define TASK2_HPP

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/srv/set_pen.hpp>
#include <std_srvs/srv/empty.hpp>

class Task2 : public rclcpp::Node
{
public:
    Task2();

    void move(double linear, double angular, double time);
    void setPen(int r, int g, int b, int width);
    void reset();

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen_;
    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr reset_;
};

#endif