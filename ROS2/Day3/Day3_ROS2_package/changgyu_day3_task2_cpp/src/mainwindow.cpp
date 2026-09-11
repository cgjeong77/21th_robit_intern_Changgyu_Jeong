#include "changgyu_day3_task2_cpp/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    node_ = std::make_shared<rclcpp::Node>("traffic_gui");

    light_sub_ =
        node_->create_subscription<std_msgs::msg::String>(
            "/traffic_light", 10,
            [this](std_msgs::msg::String::SharedPtr msg)
            {
                light_ = msg->data;
            });

    vehicle_sub_ =
        node_->create_subscription<std_msgs::msg::Float64>(
            "/vehicle_position", 10,
            [this](std_msgs::msg::Float64::SharedPtr msg)
            {
                position_ = msg->data;
            });

    timer_ = new QTimer(this);

    connect(timer_, &QTimer::timeout, this, [this]()
    {
        rclcpp::spin_some(node_);
        updateScreen();
    });

    timer_->start(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateScreen()
{
    ui->redLight->setStyleSheet(
        light_ == "RED"
        ? "background-color:red; border-radius:25px;"
        : "background-color:#500000; border-radius:25px;");

    ui->yellowLight->setStyleSheet(
        light_ == "YELLOW"
        ? "background-color:yellow; border-radius:25px;"
        : "background-color:#505000; border-radius:25px;");

    ui->greenLight->setStyleSheet(
        light_ == "GREEN"
        ? "background-color:green; border-radius:25px;"
        : "background-color:#004000; border-radius:25px;");

    ui->signalLabel->setText(
        "Signal : " + QString::fromStdString(light_));

    ui->positionLabel->setText(
        QString("Position : %1").arg(position_, 0, 'f', 2));

    int max_x =
        ui->roadFrame->width() - ui->carLabel->width() - 20;

    int x = 10;

    if (max_x > 0)
        x += static_cast<int>(
            std::fmod(position_ * 40.0, max_x));

    ui->carLabel->move(
        x,
        ui->carLabel->y());
}