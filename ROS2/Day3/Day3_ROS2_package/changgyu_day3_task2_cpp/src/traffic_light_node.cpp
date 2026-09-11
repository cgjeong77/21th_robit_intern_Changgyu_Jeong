#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <chrono>
#include <string>

using namespace std::chrono_literals;

class TrafficLight : public rclcpp::Node
{
public:
    TrafficLight() : Node("traffic_light_node"), state_("RED"), count_(0)
    {
        pub_ = create_publisher<std_msgs::msg::String>(
            "/traffic_light", 10);

        declare_parameter("red_time", 5);
        declare_parameter("green_time", 5);
        declare_parameter("yellow_time", 2);

        timer_ = create_wall_timer(
            1s,
            std::bind(&TrafficLight::timerCallback, this));
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::string state_;
    int count_;

    void timerCallback()
    {
        std_msgs::msg::String msg;
        msg.data = state_;

        pub_->publish(msg);

        RCLCPP_INFO(
            get_logger(),
            "Traffic Light : %s",
            state_.c_str());

        count_++;

        int time;

        if (state_ == "RED")
            time = get_parameter("red_time").as_int();
        else if (state_ == "GREEN")
            time = get_parameter("green_time").as_int();
        else
            time = get_parameter("yellow_time").as_int();

        if (count_ >= time)
        {
            count_ = 0;

            if (state_ == "RED")
                state_ = "GREEN";
            else if (state_ == "GREEN")
                state_ = "YELLOW";
            else
                state_ = "RED";
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(
        std::make_shared<TrafficLight>());

    rclcpp::shutdown();

    return 0;
}