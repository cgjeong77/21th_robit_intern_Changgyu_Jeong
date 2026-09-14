#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class LifecyclePublisher : public rclcpp_lifecycle::LifecycleNode
{
public:
    LifecyclePublisher()
        : LifecycleNode("lifecycle_publisher")
    {
    }

    using CallbackReturn =
        rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override
    {
        rclcpp::QoS qos(rclcpp::KeepLast(10));
        qos.reliable();
        qos.transient_local();

        publisher_ =
            this->create_publisher<std_msgs::msg::String>(
                "robot_status", qos);

        timer_ = this->create_wall_timer(
            1s,
            [this]()
            {
                std_msgs::msg::String msg;
                msg.data = "Robot is running";

                publisher_->publish(msg);

                RCLCPP_INFO(
                    this->get_logger(),
                    "Publish: %s",
                    msg.data.c_str());
            });

        timer_->cancel();

        RCLCPP_INFO(this->get_logger(), "Configured");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &) override
    {
        publisher_->on_activate();
        timer_->reset();

        RCLCPP_INFO(this->get_logger(), "Activated");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &) override
    {
        timer_->cancel();
        publisher_->on_deactivate();

        RCLCPP_INFO(this->get_logger(), "Deactivated");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override
    {
        timer_.reset();
        publisher_.reset();

        RCLCPP_INFO(this->get_logger(), "Cleaned up");

        return CallbackReturn::SUCCESS;
    }

private:
    rclcpp_lifecycle::LifecyclePublisher<
        std_msgs::msg::String>::SharedPtr publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<LifecyclePublisher>();

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node->get_node_base_interface());

    executor.spin();

    rclcpp::shutdown();

    return 0;
}

