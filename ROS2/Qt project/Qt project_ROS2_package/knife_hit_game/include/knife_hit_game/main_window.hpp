#ifndef KNIFE_HIT_GAME_MAIN_WINDOW_HPP_
#define KNIFE_HIT_GAME_MAIN_WINDOW_HPP_

#include <QMainWindow>

#include <vector>

#include "knife_hit_game/qnode.hpp"


class QGraphicsScene;
class QGraphicsItemGroup;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsTextItem;
class QGraphicsProxyWidget;
class QPushButton;
class QKeyEvent;
class QCloseEvent;
class QTimer;


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 메인 게임 창 생성
    explicit MainWindow(
        QWidget *parent = nullptr);


    // 메인 게임 창 종료
    ~MainWindow();


protected:
    // Space, R 키 입력 처리
    void keyPressEvent(
        QKeyEvent *event) override;


    // 창을 닫을 때 실행
    void closeEvent(
        QCloseEvent *event) override;


private:
    // 게임 화면 생성
    void setupGameScene();


    // 칼 발사
    void throwKnife();


    // 칼이 원판까지 이동하는 애니메이션
    void updateKnifeAnimation();


    // 원판 회전 화면 갱신
    void updateVisualRotation();


    // 원판에 꽂힌 칼 표시
    void updateEmbeddedKnives();


    // 원판에 있는 사과 표시
    void updateAppleTargets();


    // 왼쪽 남은 칼 UI 표시
    void updateRemainingKnives();


    // 현재 스테이지 UI 갱신
    void updateStageUI();


    // 게임 초기화
    void resetGame();


    // GAME OVER 화면 표시
    void showGameOver();


    // STAGE CLEAR 화면 표시
    void showStageClear();


    // YOU WIN 화면 표시
    void showWin();


    // Overlay 숨기기
    void hideOverlay();


    // Logic Node에서 받은 문자열 분석
    void parseGameState(
        const QString &state);


    // Qt Designer에서 만든 UI
    Ui::MainWindow *ui_;


    // ROS2 통신을 담당하는 QNode
    QNode *qnode_;


    // 게임 화면 Scene
    QGraphicsScene *scene_;


    // 원판, 꽂힌 칼, 사과를 함께 회전시키는 그룹
    QGraphicsItemGroup *rotating_group_;


    // 원판 이미지
    QGraphicsPixmapItem *disk_item_;


    // 발사되는 칼 이미지
    QGraphicsPixmapItem *knife_item_;


    // 원판에 꽂힌 칼 이미지 목록
    std::vector<QGraphicsPixmapItem *>
        embedded_knife_items_;


    // 현재 원판에 있는 사과 이미지 목록
    std::vector<QGraphicsPixmapItem *>
        apple_target_items_;


    // Logic Node에서 받은 칼 각도 목록
    std::vector<double>
        knife_angles_;


    // Logic Node에서 받은 사과 각도 목록
    std::vector<double>
        apple_angles_;


    // 칼 발사 애니메이션 Timer
    QTimer *knife_timer_;


    // 원판 회전 화면 갱신 Timer
    QTimer *rotation_timer_;


    // Logic Node에서 받은 실제 원판 각도
    double target_disk_angle_;


    // Qt 화면에 표시되는 원판 각도
    double visual_disk_angle_;


    // 발사 칼의 시작 Y 위치
    double knife_start_y_;


    // 발사 칼이 원판에 도착하는 Y 위치
    double knife_target_y_;


    // 현재 점수
    int score_;


    // 획득한 사과 개수
    int apple_count_;


    // 현재 스테이지
    int stage_;


    // 남은 칼 개수
    int remaining_knives_;


    // 스테이지 클리어 여부
    bool stage_clear_;


    // 최종 승리 여부
    bool win_;


    // 게임 오버 여부
    bool game_over_;

    
    // 현재 칼이 날아가는 중인지 확인
    bool knife_flying_;


    // 처음 원판 각도를 받았는지 확인
    bool first_angle_received_;


    // GAME OVER, CLEAR, WIN 화면의 어두운 배경
    QGraphicsRectItem *game_overlay_;


    // Overlay에 표시되는 글씨
    QGraphicsTextItem *overlay_text_;


    // QPushButton을 QGraphicsScene에 넣기 위한 객체
    QGraphicsProxyWidget *restart_proxy_;


    // Restart 버튼
    QPushButton *restart_button_;
};


#endif