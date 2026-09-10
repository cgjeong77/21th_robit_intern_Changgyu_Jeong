#ifndef TALKER_HPP
#define TALKER_HPP

#include <QWidget>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

QT_BEGIN_NAMESPACE
namespace Ui
{
class TalkerWindow;
}
QT_END_NAMESPACE

class Talker : public QWidget, public rclcpp::Node
{
public:
    Talker(QWidget *parent = nullptr);
    ~Talker();

private:
    Ui::TalkerWindow *ui;

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;

    void publishMessage();
};

#endif