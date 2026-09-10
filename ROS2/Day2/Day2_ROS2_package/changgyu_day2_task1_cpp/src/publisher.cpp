#include "changgyu_day2_task1_cpp/publisher.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Task1Publisher::Task1Publisher()
    : Node("task1_publisher")
{
    publisher_ =
        create_publisher<changgyu_day2_interfaces::msg::AddTwoInts>(
            "/day2/vector_data", 10);
}

void Task1Publisher::publishData(const std::vector<int32_t>& data)
{
    changgyu_day2_interfaces::msg::AddTwoInts msg;

    msg.a = static_cast<int64_t>(data.size());
    msg.b = data;

    publisher_->publish(msg);

    RCLCPP_INFO(
        get_logger(),
        "Published %ld values",
        msg.a);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Task1Publisher>();

    while (rclcpp::ok())
    {
        std::cout << "Enter numbers: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "q" || input == "Q")
            break;

        std::stringstream ss(input);
        std::vector<int32_t> data;

        int32_t value;

        while (ss >> value)
        {
            data.push_back(value);
        }

        node->publishData(data);
    }

    rclcpp::shutdown();

    return 0;
}