#include "changgyu_day2_task3_cpp/listener.hpp"
#include "ui_listener.h"

#include <QApplication>
#include <QLabel>
#include <QTimer>

Listener::Listener(QWidget *parent)
    : QWidget(parent),
      rclcpp::Node("task3_listener"),
      ui(new Ui::ListenerWindow)
{
    ui->setupUi(this);

    subscription_ =
        create_subscription<std_msgs::msg::String>(
            "/day2/chat",
            10,
            std::bind(
                &Listener::messageCallback,
                this,
                std::placeholders::_1));
}

Listener::~Listener()
{
    delete ui;
}

void Listener::messageCallback(
    const std_msgs::msg::String::SharedPtr msg)
{
    ui->messageLabel->setText(
        QString::fromStdString(msg->data));

    RCLCPP_INFO(
        get_logger(),
        "Received: %s",
        msg->data.c_str());
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    QApplication app(argc, argv);

    auto listener =
        std::make_shared<Listener>();

    listener->show();

    QTimer timer;

    QObject::connect(
        &timer,
        &QTimer::timeout,
        [listener]()
        {
            rclcpp::spin_some(listener);
        });

    timer.start(10);

    int result = app.exec();

    rclcpp::shutdown();

    return result;
}