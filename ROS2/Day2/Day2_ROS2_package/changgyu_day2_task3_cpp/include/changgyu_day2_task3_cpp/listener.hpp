#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <QWidget>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ListenerWindow;
}
QT_END_NAMESPACE

class Listener : public QWidget, public rclcpp::Node
{
public:
    Listener(QWidget *parent = nullptr);
    ~Listener();

private:
    Ui::ListenerWindow *ui;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

    void messageCallback(
        const std_msgs::msg::String::SharedPtr msg);
};

#endif