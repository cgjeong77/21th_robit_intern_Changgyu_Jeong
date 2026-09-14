#include "knife_hit_game/qnode.hpp"


QNode::QNode()
{
    // ROS2 초기화
    int argc = 0;
    char **argv = nullptr;

    rclcpp::init(
        argc,
        argv);


    // GUI에서 사용할 ROS2 Node 생성
    node_ =
        rclcpp::Node::make_shared(
            "game_gui_node");


    // Logic Node가 보내는 게임 상태를 구독
    state_sub_ =
        node_->create_subscription<std_msgs::msg::String>(
            "/game_state",
            10,
            [this](
                const std_msgs::msg::String::SharedPtr msg)
            {
                // ROS callback에서 Qt 화면을 직접 바꾸지 않고
                // Signal을 이용해서 MainWindow로 전달
                Q_EMIT gameStateReceived(
                    QString::fromStdString(
                        msg->data));
            });


    // 칼 발사 신호를 보내는 Publisher
    throw_pub_ =
        node_->create_publisher<std_msgs::msg::Empty>(
            "/knife_throw",
            10);


    // 게임 초기화 Service Client
    reset_client_ =
        node_->create_client<std_srvs::srv::Trigger>(
            "/reset_game");


    // QThread 시작
    start();
}


QNode::~QNode()
{
    // 프로그램이 종료되면 ROS2도 함께 종료
    if (rclcpp::ok())
    {
        rclcpp::shutdown();
    }


    // Thread가 완전히 종료될 때까지 기다림
    wait();
}


// ==================================================
// ROS2 Thread
// ==================================================

void QNode::run()
{
    // ROS callback을 반복적으로 처리하기 위한 반복 주기
    rclcpp::WallRate loop_rate(
        100);


    while (rclcpp::ok())
    {
        // Topic과 Service callback 처리
        rclcpp::spin_some(
            node_);


        loop_rate.sleep();
    }


    // ROS2가 종료되면 MainWindow에도 종료 신호 전달
    Q_EMIT rosShutDown();
}


// ==================================================
// 칼 발사 Topic
// ==================================================

void QNode::publishThrow()
{
    if (!rclcpp::ok())
    {
        return;
    }


    std_msgs::msg::Empty msg;


    // Logic Node에게 칼을 던졌다는 신호 전송
    throw_pub_->publish(
        msg);
}


// ==================================================
// 게임 초기화 Service
// ==================================================

bool QNode::resetGame()
{
    // Service가 아직 준비되지 않았다면 실패
    if (!reset_client_->
        service_is_ready())
    {
        RCLCPP_WARN(
            node_->get_logger(),
            "Reset service is not ready");

        return false;
    }


    auto request =
        std::make_shared<
            std_srvs::srv::Trigger::Request>();


    // Logic Node에게 게임 초기화 요청
    reset_client_->async_send_request(
        request);


    return true;
}