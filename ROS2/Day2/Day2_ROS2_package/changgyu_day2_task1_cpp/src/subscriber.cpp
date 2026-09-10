#include "changgyu_day2_task1_cpp/subscriber.hpp"

#include <iostream>
#include <functional>

Task1Subscriber::Task1Subscriber()
    : Node("task1_subscriber")
{
    subscription_ =
        create_subscription<changgyu_day2_interfaces::msg::AddTwoInts>(
            "/day2/vector_data",
            10,
            std::bind(
                &Task1Subscriber::callback,
                this,
                std::placeholders::_1));
}

void Task1Subscriber::callback(
    const changgyu_day2_interfaces::msg::AddTwoInts::SharedPtr msg)
{
    std::cout << "Received count: " << msg->a << std::endl;

    std::cout << "Received vector: ";

    for (const auto &value : msg->b)
    {
        std::cout << value << " ";
    }

    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task1Subscriber>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}