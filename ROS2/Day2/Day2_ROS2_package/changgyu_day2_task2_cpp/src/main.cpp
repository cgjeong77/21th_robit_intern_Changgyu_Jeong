#include "changgyu_day2_task2_cpp/mainwindow.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    QApplication app(argc, argv);

    auto task2 = std::make_shared<Task2>();

    MainWindow window(task2);
    window.show();

    QTimer timer;

    QObject::connect(
        &timer,
        &QTimer::timeout,
        [task2]()
        {
            rclcpp::spin_some(task2);
        });

    timer.start(10);

    int result = app.exec();

    rclcpp::shutdown();

    return result;
}