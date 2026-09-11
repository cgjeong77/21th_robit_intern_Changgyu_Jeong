#include "changgyu_day3_task1_cpp/task1.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>

using namespace std::chrono_literals;

const double PI = 3.14159265359;

Task1::Task1() : Node("task1_controller")
{
    pub_ = create_publisher<geometry_msgs::msg::Twist>(
        "/turtle1/cmd_vel", 10);

    pen_ = create_client<turtlesim::srv::SetPen>(
        "/turtle1/set_pen");

    reset_ = create_client<std_srvs::srv::Empty>(
        "/reset");

    declare_parameter("pen_r", 255);
    declare_parameter("pen_g", 0);
    declare_parameter("pen_b", 0);
    declare_parameter("pen_width", 3);
}

void Task1::move(double linear, double angular, double time)
{
    geometry_msgs::msg::Twist msg;

    msg.linear.x = linear;
    msg.angular.z = angular;

    rclcpp::Rate rate(100);

    int count = static_cast<int>(time * 100);

    for (int i = 0; i < count && rclcpp::ok(); i++)
    {
        pub_->publish(msg);
        rate.sleep();
    }

    msg.linear.x = 0.0;
    msg.angular.z = 0.0;

    pub_->publish(msg);
}

void Task1::polygon(int sides)
{
    double angle = 2.0 * PI / sides;

    for (int i = 0; i < sides; i++)
    {
        move(1.0, 0.0, 1.5);
        move(0.0, 1.0, angle);
    }
}

void Task1::circle()
{
    move(1.0, 1.0, 2.0 * PI);
}

void Task1::setPen()
{
    if (!pen_->wait_for_service(1s))
        return;

    auto req =
        std::make_shared<turtlesim::srv::SetPen::Request>();

    req->r = get_parameter("pen_r").as_int();
    req->g = get_parameter("pen_g").as_int();
    req->b = get_parameter("pen_b").as_int();
    req->width = get_parameter("pen_width").as_int();
    req->off = 0;

    pen_->async_send_request(req);

    std::this_thread::sleep_for(100ms);
}

void Task1::reset()
{
    if (!reset_->wait_for_service(1s))
    {
        std::cout << "Reset service not available\n";
        return;
    }

    auto req =
        std::make_shared<std_srvs::srv::Empty::Request>();

    auto future = reset_->async_send_request(req);

    rclcpp::spin_until_future_complete(
        shared_from_this(),
        future);

    std::cout << "Reset Complete\n";
}

char Task1::getKey()
{
    char key;
    termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    read(STDIN_FILENO, &key, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return key;
}

void Task1::run()
{
    std::cout
        << "W : Triangle\n"
        << "A : Square\n"
        << "S : Pentagon\n"
        << "D : Circle\n"
        << "R : Reset\n"
        << "Q : Quit\n";

    while (rclcpp::ok())
    {
        char key = getKey();

        switch (key)
        {
        case 'w':
        case 'W':
            setPen();
            polygon(3);
            break;

        case 'a':
        case 'A':
            setPen();
            polygon(4);
            break;

        case 's':
        case 'S':
            setPen();
            polygon(5);
            break;

        case 'd':
        case 'D':
            setPen();
            circle();
            break;

        case 'r':
        case 'R':
            reset();
            break;

        case 'q':
        case 'Q':
            return;
        }
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task1>();

    node->run();

    rclcpp::shutdown();

    return 0;
}