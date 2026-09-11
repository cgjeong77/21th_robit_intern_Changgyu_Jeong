#include "changgyu_day3_task2_cpp/mainwindow.hpp"

#include <QApplication>
#include <rclcpp/rclcpp.hpp>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    int result = app.exec();

    rclcpp::shutdown();

    return result;
}