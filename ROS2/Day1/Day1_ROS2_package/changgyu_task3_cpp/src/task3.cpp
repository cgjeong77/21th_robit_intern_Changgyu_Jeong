#include "changgyu_task3_cpp/task3.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>

using namespace std::chrono_literals;

Task3::Task3() : Node("task3_controller"), mode_(0)
{
    pub_ = create_publisher<geometry_msgs::msg::Twist>(
        "/turtle1/cmd_vel", 10);

    pen_ = create_client<turtlesim::srv::SetPen>(
        "/turtle1/set_pen");

    reset_ = create_client<std_srvs::srv::Empty>(
        "/reset");
}

void Task3::move(double linear, double angular, double time)
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

void Task3::setPen(int r, int g, int b, int width)
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

void Task3::reset()
{
    if (!reset_->wait_for_service(1s))
        return;

    auto req =
        std::make_shared<std_srvs::srv::Empty::Request>();

    reset_->async_send_request(req);

    mode_ = 0;
    std::cout << "Reset\n";
}

char Task3::getKey()
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

void Task3::run()
{
    std::cout
        << "1 : Triangle\n"
        << "2 : Square\n"
        << "3 : Circle\n"
        << "W/S : Forward/Backward\n"
        << "A/D : Left/Right\n"
        << "R : Reset\n"
        << "Q : Quit\n";

    while (rclcpp::ok())
    {
        char key = getKey();

        switch (key)
        {
        case '1':
            mode_ = 1;
            setPen(255, 0, 0, 2);
            std::cout << "Triangle Mode\n";
            break;

        case '2':
            mode_ = 2;
            setPen(0, 255, 0, 6);
            std::cout << "Square Mode\n";
            break;

        case '3':
            mode_ = 3;
            setPen(0, 0, 255, 10);
            std::cout << "Circle Mode\n";
            break;

        case 'w':
        case 'W':
            if (mode_ == 3)
                move(1.0, 1.0, 6.28);
            else
                move(1.0, 0.0, 2.0);
            break;

        case 's':
        case 'S':
            move(-1.0, 0.0, 2.0);
            break;

        case 'a':
        case 'A':
            if (mode_ == 1)
                move(0.0, 1.57, 1.33);
            else if (mode_ == 2)
                move(0.0, 1.57, 1.0);
            break;

        case 'd':
        case 'D':
            if (mode_ == 1)
                move(0.0, -1.57, 1.33);
            else if (mode_ == 2)
                move(0.0, -1.57, 1.0);
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

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task3>();
    node->run();

    rclcpp::shutdown();

    return 0;
}