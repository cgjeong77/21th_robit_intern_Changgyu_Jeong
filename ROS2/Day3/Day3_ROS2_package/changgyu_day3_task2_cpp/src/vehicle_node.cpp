#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/float64.hpp>

#include <chrono>
#include <string>

using namespace std::chrono_literals;

class Vehicle : public rclcpp::Node
{
public:
    Vehicle() : Node("vehicle_node"), position_(0.0), state_("RED")
    {
        sub_ = create_subscription<std_msgs::msg::String>(
            "/traffic_light",
            10,
            std::bind(
                &Vehicle::lightCallback,
                this,
                std::placeholders::_1));

        pub_ = create_publisher<std_msgs::msg::Float64>(
            "/vehicle_position", 10);

        declare_parameter("vehicle_speed", 1.0);

        timer_ = create_wall_timer(
            100ms,
            std::bind(&Vehicle::timerCallback, this));
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    double position_;
    std::string state_;

    void lightCallback(
        const std_msgs::msg::String::SharedPtr msg)
    {
        state_ = msg->data;
    }

    void timerCallback()
    {
        double speed =
            get_parameter("vehicle_speed").as_double();

        if (state_ == "GREEN")
            position_ += speed * 0.1;

        else if (state_ == "YELLOW")
            position_ += speed * 0.03;

        std_msgs::msg::Float64 msg;
        msg.data = position_;

        pub_->publish(msg);

        RCLCPP_INFO(
            get_logger(),
            "Light: %s | Position: %.2f",
            state_.c_str(),
            position_);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(
        std::make_shared<Vehicle>());

    rclcpp::shutdown();

    return 0;
}