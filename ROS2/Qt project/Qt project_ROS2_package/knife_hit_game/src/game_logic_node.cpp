#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>

#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <random>
#include <sstream>
#include <vector>


class GameLogicNode : public rclcpp::Node
{
public:
    GameLogicNode()
        : Node("game_logic_node"),
          disk_angle_(0.0),
          score_(0),
          game_over_(false),
          stage_(1),
          remaining_knives_(4),
          stage_clear_(false),
          stage_clear_counter_(0),
          win_(false),
          rotation_direction_(1.0),
          apple_count_(0),
          rng_(std::random_device{}())
    {
        // GUI에서 칼을 던졌다는 신호를 받는 Topic
        throw_sub_ =
            create_subscription<std_msgs::msg::Empty>(
                "/knife_throw",
                10,
                std::bind(
                    &GameLogicNode::throwKnife,
                    this,
                    std::placeholders::_1));


        // 현재 게임 상태를 GUI에게 보내는 Topic
        state_pub_ =
            create_publisher<std_msgs::msg::String>(
                "/game_state",
                10);


        // Restart 버튼을 눌렀을 때 사용하는 Service
        reset_service_ =
            create_service<std_srvs::srv::Trigger>(
                "/reset_game",
                std::bind(
                    &GameLogicNode::resetGame,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));


        // 약 16ms마다 게임 상태를 업데이트
        // 약 60FPS 정도의 속도로 동작
        timer_ =
            create_wall_timer(
                std::chrono::milliseconds(16),
                std::bind(
                    &GameLogicNode::updateGame,
                    this));


        // 처음 실행하면 Stage 1 준비
        prepareStage(1);


        RCLCPP_INFO(
            get_logger(),
            "Knife Hit Game Started");
    }


private:

    // 각도를 0~359도 범위로 맞춰주는 함수
    double normalizeAngle(double angle)
    {
        while (angle < 0.0)
            angle += 360.0;

        while (angle >= 360.0)
            angle -= 360.0;

        return angle;
    }


    // 두 각도 사이의 가장 짧은 거리 계산
    // 예: 350도와 10도는 차이가 340도가 아니라 20도
    double angleDifference(double a, double b)
    {
        double diff =
            std::fabs(a - b);

        if (diff > 180.0)
            diff = 360.0 - diff;

        return diff;
    }


    // min~max 범위에서 랜덤 정수 생성
    int randomInt(int min_value, int max_value)
    {
        std::uniform_int_distribution<int> dist(
            min_value,
            max_value);

        return dist(rng_);
    }


    // 0~360도 사이에서 랜덤 각도 생성
    double randomAngle()
    {
        std::uniform_real_distribution<double> dist(
            0.0,
            359.999);

        return dist(rng_);
    }


    // 새로운 물체가 기존 물체와 너무 가까운지 확인
    bool isAngleSafe(
        double candidate,
        const std::vector<double> &angles,
        double min_distance)
    {
        for (double angle : angles)
        {
            if (angleDifference(
                    candidate,
                    angle) <
                min_distance)
            {
                return false;
            }
        }

        return true;
    }


    // 기존 칼이나 사과와 겹치지 않는 랜덤 위치 생성
    double createSafeRandomAngle(
        const std::vector<double> &angles,
        double min_distance)
    {
        // 우선 랜덤 위치를 최대 500번 확인
        for (int i = 0; i < 500; i++)
        {
            double angle =
                randomAngle();

            if (isAngleSafe(
                    angle,
                    angles,
                    min_distance))
            {
                return angle;
            }
        }


        // 랜덤으로 위치를 찾지 못하면
        // 5도씩 이동하면서 가능한 위치 탐색
        for (double angle = 0.0;
             angle < 360.0;
             angle += 5.0)
        {
            if (isAngleSafe(
                    angle,
                    angles,
                    min_distance))
            {
                return angle;
            }
        }


        return randomAngle();
    }


    // 스테이지마다 플레이어가 던져야 하는 칼 개수
    int knivesForStage(int stage)
    {
        switch (stage)
        {
        case 1: return 4;
        case 2: return 5;
        case 3: return 6;
        case 4: return 7;
        case 5: return 8;
        default: return 4;
        }
    }


    // 스테이지 시작부터 원판에 꽂혀있는 장애물 칼 개수
    int obstacleKnivesForStage(int stage)
    {
        switch (stage)
        {
        case 1: return 0;
        case 2: return 1;
        case 3: return 2;
        case 4: return 3;
        case 5: return 3;
        default: return 0;
        }
    }


    // 스테이지마다 등장하는 사과 개수
    int applesForStage(int stage)
    {
        switch (stage)
        {
        case 1:
            return 1;

        case 2:
            return randomInt(1, 2);

        case 3:
            return 2;

        case 4:
            return randomInt(2, 3);

        case 5:
            return randomInt(3, 4);

        default:
            return 1;
        }
    }


    // 스테이지별 원판 회전 속도
    double speedForStage(int stage)
    {
        switch (stage)
        {
        case 1: return 0.40;
        case 2: return 0.65;
        case 3: return 0.90;
        case 4: return 1.20;
        case 5: return 1.55;
        default: return 0.40;
        }
    }


    // 스테이지 시작 시 장애물 칼 생성
    void createObstacleKnives(int count)
    {
        knife_angles_.clear();


        for (int i = 0; i < count; i++)
        {
            // 장애물 칼끼리 너무 가까이 생기지 않도록
            // 최소 42도 간격 유지
            double angle =
                createSafeRandomAngle(
                    knife_angles_,
                    42.0);


            knife_angles_.push_back(
                angle);
        }
    }


    // 스테이지 시작 시 사과 생성
    void createApples(int count)
    {
        apple_angles_.clear();


        for (int i = 0; i < count; i++)
        {
            // 장애물 칼의 위치를 먼저 복사
            std::vector<double> blocked =
                knife_angles_;


            // 이미 생성된 사과 위치도 추가
            blocked.insert(
                blocked.end(),
                apple_angles_.begin(),
                apple_angles_.end());


            // 칼이나 다른 사과와 겹치지 않는 위치 생성
            double angle =
                createSafeRandomAngle(
                    blocked,
                    35.0);


            apple_angles_.push_back(
                angle);
        }
    }


    // 새로운 스테이지를 준비하는 함수
    void prepareStage(int stage)
    {
        // 원판 각도 초기화
        disk_angle_ =
            0.0;


        // 해당 스테이지에서 던져야 하는 칼 개수 설정
        remaining_knives_ =
            knivesForStage(stage);


        stage_clear_ =
            false;


        stage_clear_counter_ =
            0;


        // Stage 3과 Stage 5는 반대 방향으로 회전
        // 한 스테이지 안에서는 방향이 바뀌지 않음
        if (stage == 3 ||
            stage == 5)
        {
            rotation_direction_ =
                -1.0;
        }
        else
        {
            rotation_direction_ =
                1.0;
        }


        // 장애물 칼 생성
        createObstacleKnives(
            obstacleKnivesForStage(stage));


        // 사과 생성
        createApples(
            applesForStage(stage));


        RCLCPP_INFO(
            get_logger(),
            "Stage %d | Knives:%d | Obstacles:%zu | Apples:%zu",
            stage_,
            remaining_knives_,
            knife_angles_.size(),
            apple_angles_.size());
    }


    // 게임 상태를 계속 갱신하는 함수
    void updateGame()
    {
        // 스테이지 클리어 상태라면
        if (stage_clear_)
        {
            stage_clear_counter_++;


            // 약 1.4초 후 다음 스테이지로 이동
            if (stage_clear_counter_ >= 90)
            {
                goToNextStage();
            }


            publishState();

            return;
        }


        // 게임 오버나 승리 상태가 아니라면 원판 회전
        if (!game_over_ &&
            !win_)
        {
            disk_angle_ +=
                speedForStage(stage_) *
                rotation_direction_;


            disk_angle_ =
                normalizeAngle(
                    disk_angle_);
        }


        // GUI에게 현재 상태 전달
        publishState();
    }


    // 플레이어가 칼을 던졌을 때 실행
    void throwKnife(
        const std_msgs::msg::Empty::SharedPtr)
    {
        // 이미 게임이 끝났다면 입력 무시
        if (game_over_ ||
            stage_clear_ ||
            win_)
        {
            return;
        }


        // 현재 원판 각도를 기준으로
        // 새 칼이 꽂힐 상대 각도를 계산
        double new_angle =
            normalizeAngle(
                -disk_angle_);


        // 기존 칼과 너무 가까우면 충돌
        for (double old_angle :
             knife_angles_)
        {
            if (angleDifference(
                    new_angle,
                    old_angle) <
                18.0)
            {
                game_over_ =
                    true;


                publishState();

                return;
            }
        }


        // 칼이 사과 위치를 맞췄는지 확인
        for (auto it =
                 apple_angles_.begin();
             it != apple_angles_.end();
             ++it)
        {
            if (angleDifference(
                    new_angle,
                    *it) <
                28.0)
            {
                // 사과 개수 증가
                apple_count_++;


                // 맞은 사과는 제거
                apple_angles_.erase(
                    it);


                break;
            }
        }


        // 정상적으로 꽂힌 칼의 각도 저장
        knife_angles_.push_back(
            new_angle);


        // 점수 증가
        score_++;


        // 남은 칼 개수 감소
        remaining_knives_--;


        // 모든 칼을 사용했다면 스테이지 종료
        if (remaining_knives_ <= 0)
        {
            // Stage 5까지 끝내면 최종 승리
            if (stage_ == 5)
            {
                win_ =
                    true;
            }
            else
            {
                stage_clear_ =
                    true;


                stage_clear_counter_ =
                    0;
            }
        }


        publishState();
    }


    // 다음 스테이지로 이동
    void goToNextStage()
    {
        stage_++;


        prepareStage(
            stage_);
    }


    // Restart Service 처리
    void resetGame(
        const std::shared_ptr<
            std_srvs::srv::Trigger::Request>,
        std::shared_ptr<
            std_srvs::srv::Trigger::Response> response)
    {
        // 점수 초기화
        score_ =
            0;


        // 획득한 사과 개수도 초기화
        apple_count_ =
            0;


        game_over_ =
            false;


        win_ =
            false;


        stage_ =
            1;


        // Stage 1을 새롭게 다시 생성
        // 사과와 장애물 위치도 다시 랜덤 생성됨
        prepareStage(
            1);


        response->success =
            true;


        response->message =
            "Game reset";


        publishState();
    }


    // 게임의 현재 상태를 문자열로 만들어 GUI에 전송
    void publishState()
    {
        std_msgs::msg::String message;


        std::ostringstream stream;


        stream
            << "angle="
            << disk_angle_

            << ";score="
            << score_

            << ";game_over="
            << (game_over_ ? 1 : 0)

            << ";stage="
            << stage_

            << ";remaining="
            << remaining_knives_

            << ";stage_clear="
            << (stage_clear_ ? 1 : 0)

            << ";win="
            << (win_ ? 1 : 0)

            << ";apple_count="
            << apple_count_

            << ";knives=";


        // 꽂힌 칼의 각도들을 쉼표로 연결해서 전송
        for (std::size_t i = 0;
             i < knife_angles_.size();
             i++)
        {
            stream
                << knife_angles_[i];


            if (i + 1 <
                knife_angles_.size())
            {
                stream
                    << ",";
            }
        }


        stream
            << ";apples=";


        // 현재 남아있는 사과 각도들을 전송
        for (std::size_t i = 0;
             i < apple_angles_.size();
             i++)
        {
            stream
                << apple_angles_[i];


            if (i + 1 <
                apple_angles_.size())
            {
                stream
                    << ",";
            }
        }


        message.data =
            stream.str();


        state_pub_->publish(
            message);
    }


    // ROS 통신 객체
    rclcpp::Subscription<
        std_msgs::msg::Empty>::SharedPtr
        throw_sub_;


    rclcpp::Publisher<
        std_msgs::msg::String>::SharedPtr
        state_pub_;


    rclcpp::Service<
        std_srvs::srv::Trigger>::SharedPtr
        reset_service_;


    rclcpp::TimerBase::SharedPtr
        timer_;


    // 원판의 현재 각도
    double disk_angle_;


    // 게임 점수
    int score_;


    // 게임 오버 여부
    bool game_over_;


    // 현재 스테이지
    int stage_;


    // 남은 발사 칼 개수
    int remaining_knives_;


    // 스테이지 클리어 여부
    bool stage_clear_;


    // 다음 스테이지로 넘어가기 위한 시간 카운트
    int stage_clear_counter_;


    // 최종 승리 여부
    bool win_;


    // 원판 회전 방향
    // 1 = 정방향, -1 = 역방향
    double rotation_direction_;


    // 원판에 꽂혀있는 칼의 각도
    std::vector<double>
        knife_angles_;


    // 획득한 사과 개수
    int apple_count_;


    // 현재 원판에 있는 사과의 각도
    std::vector<double>
        apple_angles_;


    // 랜덤 값 생성기
    std::mt19937
        rng_;
};


int main(
    int argc,
    char *argv[])
{
    // ROS2 시작
    rclcpp::init(
        argc,
        argv);


    // GameLogicNode 객체 생성
    auto node =
        std::make_shared<
            GameLogicNode>();


    // 노드 실행
    rclcpp::spin(
        node);


    // ROS2 종료
    rclcpp::shutdown();


    return 0;
}