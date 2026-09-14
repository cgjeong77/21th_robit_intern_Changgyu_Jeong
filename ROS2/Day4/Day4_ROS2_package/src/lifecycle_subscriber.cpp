#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

class LifecycleSubscriber : public rclcpp_lifecycle::LifecycleNode
{
public:
    LifecycleSubscriber()
        : LifecycleNode("lifecycle_subscriber")
    {
    }

    using CallbackReturn =
        rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override
    {
        rclcpp::QoS qos(rclcpp::KeepLast(10));
        qos.reliable();
        qos.transient_local();

        subscription_ =
            this->create_subscription<std_msgs::msg::String>(
                "robot_status",
                qos,
                [this](const std_msgs::msg::String::SharedPtr msg)
                {
                    if (active_)
                    {
                        RCLCPP_INFO(
                            this->get_logger(),
                            "Receive: %s",
                            msg->data.c_str());
                    }
                });

        RCLCPP_INFO(this->get_logger(), "Configured");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &) override
    {
        active_ = true;

        RCLCPP_INFO(this->get_logger(), "Activated");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &) override
    {
        active_ = false;

        RCLCPP_INFO(this->get_logger(), "Deactivated");

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override
    {
        active_ = false;
        subscription_.reset();

        RCLCPP_INFO(this->get_logger(), "Cleaned up");

        return CallbackReturn::SUCCESS;
    }

private:
    bool active_ = false;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<LifecycleSubscriber>();

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node->get_node_base_interface());

    executor.spin();

    rclcpp::shutdown();

    return 0;
}
