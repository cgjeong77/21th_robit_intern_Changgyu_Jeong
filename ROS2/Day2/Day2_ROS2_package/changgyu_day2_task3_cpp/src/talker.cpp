#include "changgyu_day2_task3_cpp/talker.hpp"
#include "ui_talker.h"

#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>

Talker::Talker(QWidget *parent)
    : QWidget(parent),
      rclcpp::Node("task3_talker"),
      ui(new Ui::TalkerWindow)
{
    ui->setupUi(this);

    publisher_ =
        create_publisher<std_msgs::msg::String>(
            "/day2/chat",
            10);

    connect(
        ui->publishButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            publishMessage();
        });
}

Talker::~Talker()
{
    delete ui;
}

void Talker::publishMessage()
{
    std_msgs::msg::String msg;

    msg.data =
        ui->messageEdit->text().toStdString();

    publisher_->publish(msg);

    RCLCPP_INFO(
        get_logger(),
        "Published: %s",
        msg.data.c_str());
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    QApplication app(argc, argv);

    auto talker =
        std::make_shared<Talker>();

    talker->show();

    QTimer timer;

    QObject::connect(
        &timer,
        &QTimer::timeout,
        [talker]()
        {
            rclcpp::spin_some(talker);
        });

    timer.start(10);

    int result = app.exec();

    rclcpp::shutdown();

    return result;
}