#ifndef TASK1_HPP
#define TASK1_HPP

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/srv/set_pen.hpp>
#include <std_srvs/srv/empty.hpp>

class Task1 : public rclcpp::Node
{
public:
    Task1();
    void run();

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen_;
    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr reset_;

    void move(double linear, double angular, double time);
    void polygon(int sides);
    void circle();
    void setPen();
    void reset();
    char getKey();
};

#endif