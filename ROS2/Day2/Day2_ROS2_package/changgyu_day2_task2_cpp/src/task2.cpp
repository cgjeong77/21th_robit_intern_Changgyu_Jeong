#include "changgyu_day2_task2_cpp/task2.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

Task2::Task2()
    : Node("task2_controller")
{
    pub_ = create_publisher<geometry_msgs::msg::Twist>(
        "/turtle1/cmd_vel", 10);

    pen_ = create_client<turtlesim::srv::SetPen>(
        "/turtle1/set_pen");

    reset_ = create_client<std_srvs::srv::Empty>(
        "/reset");
}

void Task2::move(double linear, double angular, double time)
{
    geometry_msgs::msg::Twist msg;

    msg.linear.x = linear;
    msg.angular.z = angular;

    auto start = std::chrono::steady_clock::now();

    while (rclcpp::ok())
    {
        double t = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - start).count();

        if (t >= time)
            break;

        pub_->publish(msg);

        std::this_thread::sleep_for(50ms);
    }

    msg.linear.x = 0.0;
    msg.angular.z = 0.0;

    pub_->publish(msg);
}

void Task2::setPen(int r, int g, int b, int width)
{
    if (!pen_->wait_for_service(1s))
        return;

    auto req =
        std::make_shared<turtlesim::srv::SetPen::Request>();

    req->r = r;
    req->g = g;
    req->b = b;
    req->width = width;
    req->off = 0;

    pen_->async_send_request(req);
}

void Task2::reset()
{
    if (!reset_->wait_for_service(1s))
        return;

    auto req =
        std::make_shared<std_srvs::srv::Empty::Request>();

    reset_->async_send_request(req);

    RCLCPP_INFO(
        get_logger(),
        "Turtlesim Reset");
}