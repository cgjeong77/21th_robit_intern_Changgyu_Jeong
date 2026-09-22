#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

#include <cmath>
#include <functional>
#include <vector>

class YellowBallDetector : public rclcpp::Node
{
public:
    YellowBallDetector() : Node("yellow_ball_detector")
    {
        subscription_ = create_subscription<sensor_msgs::msg::Image>(
            "/camera1/camera/compressed_image",
            rclcpp::SensorDataQoS(),
            std::bind(
                &YellowBallDetector::imageCallback,
                this,
                std::placeholders::_1));

        cv::namedWindow("Yellow Ball Distance", cv::WINDOW_NORMAL);

        RCLCPP_INFO(get_logger(), "Yellow Ball Detector Started");
    }

private:
    // Camera parameters
    const double fx_ = 471.953641;
    const double fy_ = 476.574144;
    const double cy_ = 228.222101;

    // Measured values
    const double ballDiameter_ = 6.5;   // cm
    const double cameraHeight_ = 5.5;   // cm
    const double cameraTilt_ = 0.0;     // degree

    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        cv::Mat frame;

        try
        {
            frame = cv_bridge::toCvCopy(msg, "bgr8")->image;
        }
        catch (const cv_bridge::Exception &e)
        {
            RCLCPP_ERROR(get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        cv::Mat hsv;
        cv::Mat mask;

        // Yellow color detection
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        cv::inRange(
            hsv,
            cv::Scalar(23, 120, 100),
            cv::Scalar(38, 255, 255),
            mask);

        // Noise removal
        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_ELLIPSE,
            cv::Size(5, 5));

        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

        // Find contours
        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(
            mask,
            contours,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE);

        int ballIndex = -1;
        double maxArea = 0.0;

        for (int i = 0; i < static_cast<int>(contours.size()); i++)
        {
            double area = cv::contourArea(contours[i]);
            double perimeter = cv::arcLength(contours[i], true);

            if (area < 100.0 || perimeter <= 0.0)
                continue;

            double circularity =
                4.0 * CV_PI * area / (perimeter * perimeter);

            if (circularity < 0.55)
                continue;

            if (area > maxArea)
            {
                maxArea = area;
                ballIndex = i;
            }
        }

        if (ballIndex != -1)
        {
            cv::Point2f center;
            float radius;

            cv::minEnclosingCircle(
                contours[ballIndex],
                center,
                radius);

            if (radius > 5.0f)
            {
                // Find the bottom point of the ball
                cv::Point bottomPoint = contours[ballIndex][0];

                for (const auto &point : contours[ballIndex])
                {
                    if (point.y > bottomPoint.y)
                        bottomPoint = point;
                }

                // Method A
                double pixelDiameter = 2.0 * radius;

                double distanceA =
                    fx_ * ballDiameter_ / pixelDiameter;

                // Method B
                double yn =
                    (static_cast<double>(bottomPoint.y) - cy_) / fy_;

                double alpha = std::atan(yn);
                double theta = cameraTilt_ * CV_PI / 180.0;
                double angle = theta + alpha;

                double distanceB = -1.0;

                if (angle > 0.001)
                {
                    distanceB =
                        cameraHeight_ / std::tan(angle);
                }

                // Draw detected ball
                cv::circle(
                    frame,
                    center,
                    static_cast<int>(radius),
                    cv::Scalar(0, 255, 0),
                    3);

                cv::circle(
                    frame,
                    center,
                    5,
                    cv::Scalar(0, 0, 255),
                    -1);

                cv::circle(
                    frame,
                    bottomPoint,
                    5,
                    cv::Scalar(255, 0, 0),
                    -1);

                // Display Method A
                cv::putText(
                    frame,
                    cv::format("Method A : %.2f cm", distanceA),
                    cv::Point(20, 35),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.7,
                    cv::Scalar(0, 255, 0),
                    2);

                // Display Method B
                if (distanceB > 0.0)
                {
                    cv::putText(
                        frame,
                        cv::format("Method B : %.2f cm", distanceB),
                        cv::Point(20, 70),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.7,
                        cv::Scalar(255, 0, 0),
                        2);
                }

                // Display detected pixel diameter
                cv::putText(
                    frame,
                    cv::format("Diameter : %.1f px", pixelDiameter),
                    cv::Point(20, 105),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(255, 255, 255),
                    1);
            }
        }

        cv::imshow("Yellow Ball Distance", frame);
        cv::waitKey(1);
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<YellowBallDetector>();
    rclcpp::spin(node);

    cv::destroyAllWindows();
    rclcpp::shutdown();

    return 0;
}