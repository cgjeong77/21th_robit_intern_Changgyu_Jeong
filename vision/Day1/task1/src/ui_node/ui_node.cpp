#include "camera_ui_pkg/ui_node/ui_node.hpp"
#include "ui_camera_ui.h"

#include <QApplication>
#include <QImage>
#include <QPixmap>

#include <functional>
#include <memory>

UINode::UINode()
: Node("ui_node")
{
    declare_parameter<std::string>("topic", "/camera/image");
    declare_parameter<double>("hz", 30.0);

    topic_ = get_parameter("topic").as_string();
    hz_ = get_parameter("hz").as_double();

    if (hz_ <= 0.0) {
        hz_ = 30.0;
    }

    image_sub_ = create_subscription<sensor_msgs::msg::Image>(
        topic_, 10,
        std::bind(&UINode::imageCallback, this, std::placeholders::_1)
    );

    window_ = new QWidget();
    ui_ = new Ui::CameraUI();
    ui_->setupUi(window_);

    ui_timer_ = new QTimer(window_);

    QObject::connect(ui_timer_, &QTimer::timeout, [this]() {
        updateUI();
    });

    ui_timer_->start(static_cast<int>(1000.0 / hz_));
    window_->show();

    RCLCPP_INFO(get_logger(), "UI node started");
    RCLCPP_INFO(get_logger(), "Topic: %s", topic_.c_str());
    RCLCPP_INFO(get_logger(), "Hz: %.1f", hz_);
}

UINode::~UINode()
{
    delete ui_;
    delete window_;
}

void UINode::imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
{
    try {
        cv::Mat received_image = cv_bridge::toCvCopy(msg, "bgr8")->image;

        std::lock_guard<std::mutex> lock(image_mutex_);
        image_ = received_image.clone();
    }
    catch (const cv_bridge::Exception &e) {
        RCLCPP_ERROR(get_logger(), "cv_bridge exception: %s", e.what());
    }
}

void UINode::updateUI()
{
    cv::Mat frame;

    {
        std::lock_guard<std::mutex> lock(image_mutex_);

        if (image_.empty()) {
            return;
        }

        frame = image_.clone();
    }

    cv::Mat rgb;
    cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

    QImage qimage(
        rgb.data,
        rgb.cols,
        rgb.rows,
        static_cast<int>(rgb.step),
        QImage::Format_RGB888
    );

    QPixmap pixmap = QPixmap::fromImage(qimage.copy());

    ui_->imageLabel->setPixmap(
        pixmap.scaled(
            ui_->imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);

    auto node = std::make_shared<UINode>();

    QTimer ros_timer;

    QObject::connect(&ros_timer, &QTimer::timeout, [&node]() {
        rclcpp::spin_some(node);
    });

    ros_timer.start(10);

    int result = app.exec();

    rclcpp::shutdown();
    return result;
}