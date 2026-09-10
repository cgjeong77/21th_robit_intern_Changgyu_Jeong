#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QKeyEvent>

#include <memory>

#include "changgyu_day2_task2_cpp/task2.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
public:
    MainWindow(
        const std::shared_ptr<Task2> &task2,
        QWidget *parent = nullptr);

    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Task2> task2_;

    int mode_;

    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();

    void triangleMode();
    void squareMode();
    void circleMode();

    void resetTurtle();

    void updateVelocity(double linear, double angular);
};

#endif