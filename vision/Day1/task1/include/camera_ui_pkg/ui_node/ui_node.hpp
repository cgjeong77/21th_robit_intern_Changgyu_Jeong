#ifndef CAMERA_UI_PKG_UI_NODE_UI_NODE_HPP_
#define CAMERA_UI_PKG_UI_NODE_UI_NODE_HPP_

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

#include <QTimer>
#include <QWidget>

#include <mutex>
#include <string>

namespace Ui {
class CameraUI;
}

class UINode : public rclcpp::Node
{
public:
    UINode();
    ~UINode();

private:
    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg);
    void updateUI();

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;

    QWidget *window_;
    Ui::CameraUI *ui_;
    QTimer *ui_timer_;

    cv::Mat image_;
    std::mutex image_mutex_;

    std::string topic_;
    double hz_;
};

#endif