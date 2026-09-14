#ifndef KNIFE_HIT_GAME_QNODE_HPP_
#define KNIFE_HIT_GAME_QNODE_HPP_

#include <QThread>
#include <QString>

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>


// ROS2 통신을 담당하는 클래스
// QThread를 상속하여 Qt 화면과 ROS2 spin을 분리해서 실행
class QNode : public QThread
{
    Q_OBJECT

public:
    QNode();
    ~QNode() override;


    // Logic Node에게 칼 발사 신호 전송
    void publishThrow();


    // Logic Node에게 게임 초기화 요청
    bool resetGame();


protected:
    // 별도의 Thread에서 ROS2 callback 처리
    void run() override;


Q_SIGNALS:

    // ROS2가 종료되었을 때 MainWindow에 전달
    void rosShutDown();


    // Logic Node에서 받은 게임 상태를 MainWindow에 전달
    void gameStateReceived(
        const QString &state);


private:

    // GUI에서 사용하는 ROS2 Node
    std::shared_ptr<rclcpp::Node>
        node_;


    // /game_state Topic을 받는 Subscriber
    rclcpp::Subscription<
        std_msgs::msg::String>::SharedPtr
        state_sub_;


    // /knife_throw Topic을 보내는 Publisher
    rclcpp::Publisher<
        std_msgs::msg::Empty>::SharedPtr
        throw_pub_;


    // /reset_game Service를 요청하는 Client
    rclcpp::Client<
        std_srvs::srv::Trigger>::SharedPtr
        reset_client_;
};


#endif