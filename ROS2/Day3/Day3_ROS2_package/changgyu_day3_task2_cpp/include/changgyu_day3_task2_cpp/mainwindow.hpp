#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/float64.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    rclcpp::Node::SharedPtr node_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr light_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr vehicle_sub_;

    QTimer *timer_;

    std::string light_ = "RED";
    double position_ = 0.0;

    void updateScreen();
};

#endif