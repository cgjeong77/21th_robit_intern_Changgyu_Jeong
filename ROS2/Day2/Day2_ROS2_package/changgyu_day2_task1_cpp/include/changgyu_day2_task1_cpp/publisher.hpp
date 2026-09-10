#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include <rclcpp/rclcpp.hpp>
#include <changgyu_day2_interfaces/msg/add_two_ints.hpp>

#include <vector>

class Task1Publisher : public rclcpp::Node
{
public:
    Task1Publisher();

    void publishData(const std::vector<int32_t>& data);

private:
    rclcpp::Publisher<
        changgyu_day2_interfaces::msg::AddTwoInts>::SharedPtr publisher_;
};

#endif