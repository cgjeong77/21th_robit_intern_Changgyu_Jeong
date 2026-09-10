#include "changgyu_day2_task2_cpp/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QApplication>

MainWindow::MainWindow(
    const std::shared_ptr<Task2> &task2,
    QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MainWindow),
      task2_(task2),
      mode_(0)
{
    ui->setupUi(this);

    connect(
        ui->wButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            moveForward();
        });

    connect(
        ui->aButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            turnLeft();
        });

    connect(
        ui->sButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            moveBackward();
        });

    connect(
        ui->dButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            turnRight();
        });

    connect(
        ui->triangleButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            triangleMode();
        });

    connect(
        ui->squareButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            squareMode();
        });

    connect(
        ui->circleButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            circleMode();
        });

    connect(
        ui->resetButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            resetTurtle();
        });

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateVelocity(
    double linear,
    double angular)
{
    ui->linearLabel->setText(
        QString("linear.x: %1").arg(linear));

    ui->angularLabel->setText(
        QString("angular.z: %1").arg(angular));

    QApplication::processEvents();
}

void MainWindow::moveForward()
{
    if (mode_ == 3)
    {
        updateVelocity(1.0, 1.0);
        task2_->move(1.0, 1.0, 6.28);
    }
    else
    {
        updateVelocity(1.0, 0.0);
        task2_->move(1.0, 0.0, 2.0);
    }

    updateVelocity(0.0, 0.0);
    setFocus();
}

void MainWindow::moveBackward()
{
    updateVelocity(-1.0, 0.0);

    task2_->move(
        -1.0,
        0.0,
        2.0);

    updateVelocity(0.0, 0.0);
    setFocus();
}

void MainWindow::turnLeft()
{
    updateVelocity(0.0, 1.57);

    if (mode_ == 1)
    {
        task2_->move(
            0.0,
            1.57,
            1.33);
    }
    else
    {
        task2_->move(
            0.0,
            1.57,
            1.0);
    }

    updateVelocity(0.0, 0.0);
    setFocus();
}

void MainWindow::turnRight()
{
    updateVelocity(0.0, -1.57);

    if (mode_ == 1)
    {
        task2_->move(
            0.0,
            -1.57,
            1.33);
    }
    else
    {
        task2_->move(
            0.0,
            -1.57,
            1.0);
    }

    updateVelocity(0.0, 0.0);
    setFocus();
}

void MainWindow::triangleMode()
{
    mode_ = 1;

    ui->modeLabel->setText(
        "Mode: Triangle");

    task2_->setPen(
        255,
        0,
        0,
        2);

    setFocus();
}

void MainWindow::squareMode()
{
    mode_ = 2;

    ui->modeLabel->setText(
        "Mode: Square");

    task2_->setPen(
        0,
        255,
        0,
        6);

    setFocus();
}

void MainWindow::circleMode()
{
    mode_ = 3;

    ui->modeLabel->setText(
        "Mode: Circle");

    task2_->setPen(
        0,
        0,
        255,
        10);

    setFocus();
}

void MainWindow::resetTurtle()
{
    task2_->reset();

    mode_ = 0;

    ui->modeLabel->setText(
        "Mode: None");

    updateVelocity(0.0, 0.0);

    setFocus();
}

void MainWindow::keyPressEvent(
    QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        moveForward();
        break;

    case Qt::Key_S:
        moveBackward();
        break;

    case Qt::Key_A:
        turnLeft();
        break;

    case Qt::Key_D:
        turnRight();
        break;

    case Qt::Key_1:
        triangleMode();
        break;

    case Qt::Key_2:
        squareMode();
        break;

    case Qt::Key_3:
        circleMode();
        break;

    case Qt::Key_R:
        resetTurtle();
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }
}