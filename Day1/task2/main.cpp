#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main()
{
    cv::Mat image = cv::imread("images/sample.png");

    if (image.empty()) {
        std::cout << "Image load failed" << std::endl;
        return 1;
    }

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    cv::Mat red1, red2, red;
    cv::Mat blue, green;

    // Red
    cv::inRange(hsv, cv::Scalar(0, 80, 50),
                cv::Scalar(10, 255, 255), red1);

    cv::inRange(hsv, cv::Scalar(170, 80, 50),
                cv::Scalar(179, 255, 255), red2);

    cv::bitwise_or(red1, red2, red);

    // Blue
    cv::inRange(hsv, cv::Scalar(90, 70, 40),
                cv::Scalar(135, 255, 255), blue);

    // Green
    cv::inRange(hsv, cv::Scalar(35, 60, 30),
                cv::Scalar(90, 255, 255), green);

    cv::Mat kernel =
        cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

    cv::morphologyEx(red, red, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(red, red, cv::MORPH_CLOSE, kernel);

    cv::morphologyEx(blue, blue, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(blue, blue, cv::MORPH_CLOSE, kernel);

    cv::morphologyEx(green, green, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(green, green, cv::MORPH_CLOSE, kernel);

    std::vector<std::vector<cv::Point>> red_contours;
    std::vector<std::vector<cv::Point>> blue_contours;
    std::vector<std::vector<cv::Point>> green_contours;

    cv::findContours(red, red_contours,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::findContours(blue, blue_contours,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::findContours(green, green_contours,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto &contour : red_contours) {
        cv::Rect box = cv::boundingRect(contour);
        cv::rectangle(image, box, cv::Scalar(0, 0, 255), 2);
    }

    for (const auto &contour : blue_contours) {
        cv::Rect box = cv::boundingRect(contour);
        cv::rectangle(image, box, cv::Scalar(255, 0, 0), 2);
    }

    for (const auto &contour : green_contours) {
        cv::Rect box = cv::boundingRect(contour);
        cv::rectangle(image, box, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("Result", image);
    cv::imwrite("result.png", image);

    while (true) {
        int key = cv::waitKey(1);

        if (key == 27) {
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}