#include "knife_hit_game/main_window.hpp"
#include "ui_mainwindow.h"

#include <QBrush>
#include <QCloseEvent>
#include <QColor>
#include <QFont>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPointF>
#include <QPushButton>
#include <QRectF>
#include <QStringList>
#include <QTimer>

#include <algorithm>
#include <cmath>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      qnode_(nullptr),
      scene_(nullptr),
      rotating_group_(nullptr),
      disk_item_(nullptr),
      knife_item_(nullptr),
      knife_timer_(nullptr),
      rotation_timer_(nullptr),
      target_disk_angle_(0.0),
      visual_disk_angle_(0.0),
      knife_start_y_(485.0),
      knife_target_y_(316.0),
      score_(0),
      apple_count_(0),
      stage_(1),
      remaining_knives_(4),
      stage_clear_(false),
      win_(false),
      game_over_(false),
      knife_flying_(false),
      first_angle_received_(false),
      game_overlay_(nullptr),
      overlay_text_(nullptr),
      restart_proxy_(nullptr),
      restart_button_(nullptr)
{
    // Qt Designer에서 만든 UI 불러오기
    ui_->setupUi(this);


    // 게임 화면 생성
    setupGameScene();


    // ROS2 통신을 담당하는 QNode 생성
    // QNode 내부의 QThread에서 ROS2 spin이 실행됨
    qnode_ =
        new QNode();


    // QNode가 받은 /game_state 데이터를 MainWindow로 전달
    connect(
        qnode_,
        &QNode::gameStateReceived,
        this,
        [this](const QString &state)
        {
            parseGameState(state);
        });


    // ROS2가 종료되면 Qt 창도 종료
    connect(
        qnode_,
        &QNode::rosShutDown,
        this,
        &MainWindow::close);


    // ==================================================
    // 칼 발사 애니메이션 Timer
    // ==================================================

    knife_timer_ =
        new QTimer(this);


    knife_timer_->setInterval(
        16);


    connect(
        knife_timer_,
        &QTimer::timeout,
        this,
        [this]()
        {
            updateKnifeAnimation();
        });


    // ==================================================
    // 원판 회전 화면 갱신 Timer
    // ==================================================

    rotation_timer_ =
        new QTimer(this);


    rotation_timer_->setInterval(
        16);


    connect(
        rotation_timer_,
        &QTimer::timeout,
        this,
        [this]()
        {
            updateVisualRotation();
        });


    rotation_timer_->start();


    // 키보드 입력을 받을 수 있도록 설정
    setFocusPolicy(
        Qt::StrongFocus);


    setFocus();


    updateRemainingKnives();
    updateStageUI();
}


MainWindow::~MainWindow()
{
    // QNode를 삭제하면 내부 QThread와 ROS2도 같이 종료됨
    delete qnode_;

    delete ui_;
}


// ==================================================
// 게임 화면 생성
// ==================================================

void MainWindow::setupGameScene()
{
    // QGraphicsView에서 사용할 Scene 생성
    scene_ =
        new QGraphicsScene(this);


    ui_->gameView->setScene(
        scene_);


    // 이미지의 회전과 크기 변경을 부드럽게 표시
    ui_->gameView->setRenderHints(
        QPainter::Antialiasing |
        QPainter::SmoothPixmapTransform);


    ui_->gameView->setViewportUpdateMode(
        QGraphicsView::FullViewportUpdate);


    // 게임에서 사용하는 Scene 좌표 범위
    scene_->setSceneRect(
        0,
        0,
        560,
        700);


    // ==================================================
    // 회전 그룹 생성
    // ==================================================

    // 원판, 꽂힌 칼, 사과를 하나의 그룹으로 묶음
    // 그룹 전체를 회전시키면 모든 물체가 함께 회전
    rotating_group_ =
        new QGraphicsItemGroup();


    scene_->addItem(
        rotating_group_);


    // 원판의 회전 중심 위치
    rotating_group_->setPos(
        280.0,
        210.0);


    // ==================================================
    // 원판 생성
    // ==================================================

    QPixmap disk_pixmap(
        ":/images/disk.png");


    if (!disk_pixmap.isNull())
    {
        disk_pixmap =
            disk_pixmap.scaled(
                260,
                260,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);


        disk_item_ =
            new QGraphicsPixmapItem(
                disk_pixmap,
                rotating_group_);


        QRectF rect =
            disk_item_->boundingRect();


        // 원판 중심을 회전 그룹의 중심에 맞춤
        disk_item_->setPos(
            -(rect.width() / 2.0 + 2.5),
            -(rect.height() / 2.0));


        disk_item_->setZValue(
            2);
    }


    // ==================================================
    // 발사 대기 중인 칼 생성
    // ==================================================

    QPixmap knife_pixmap(
        ":/images/knife.png");


    if (!knife_pixmap.isNull())
    {
        knife_pixmap =
            knife_pixmap.scaled(
                70,
                170,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);


        knife_item_ =
            scene_->addPixmap(
                knife_pixmap);


        QRectF rect =
            knife_item_->boundingRect();


        // 화면 아래쪽 중앙에 칼 배치
        knife_item_->setPos(
            (560.0 - rect.width()) / 2.0,
            knife_start_y_);


        knife_item_->setZValue(
            5);
    }


    // ==================================================
    // GAME OVER / CLEAR / WIN Overlay
    // ==================================================

    // 게임 화면 위에 표시할 반투명 검은 배경
    game_overlay_ =
        scene_->addRect(
            scene_->sceneRect(),
            QPen(Qt::NoPen),
            QBrush(
                QColor(
                    0,
                    0,
                    0,
                    175)));


    game_overlay_->setZValue(
        100);


    // Overlay 위에 표시되는 글씨
    overlay_text_ =
        scene_->addText(
            "");


    QFont font;

    font.setPointSize(
        30);

    font.setBold(
        true);


    overlay_text_->setFont(
        font);


    overlay_text_->setZValue(
        101);


    // ==================================================
    // Restart 버튼
    // ==================================================

    restart_button_ =
        new QPushButton(
            "RESTART");


    restart_button_->setFixedSize(
        180,
        52);


    restart_button_->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(245,181,52);"
        "color: rgb(20,20,20);"
        "font-size:18px;"
        "font-weight:bold;"
        "border:none;"
        "border-radius:12px;"
        "}"
        "QPushButton:hover {"
        "background-color: rgb(255,205,80);"
        "}");


    // 일반 QPushButton을 QGraphicsScene 안에 넣기 위해 사용
    restart_proxy_ =
        scene_->addWidget(
            restart_button_);


    restart_proxy_->setZValue(
        101);


    connect(
        restart_button_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            resetGame();
        });


    // 프로그램 시작 시 Overlay 숨김
    hideOverlay();
}


// ==================================================
// 칼 발사 시작
// ==================================================

void MainWindow::throwKnife()
{
    // 게임 오버, 클리어, 승리 상태이거나
    // 이미 칼이 날아가는 중이면 다시 발사하지 않음
    if (game_over_ ||
        stage_clear_ ||
        win_ ||
        knife_flying_ ||
        !knife_item_)
    {
        return;
    }


    knife_flying_ =
        true;


    knife_timer_->start();
}


// ==================================================
// 칼 발사 애니메이션
// ==================================================

void MainWindow::updateKnifeAnimation()
{
    if (!knife_flying_ ||
        !knife_item_)
    {
        return;
    }


    QPointF pos =
        knife_item_->pos();


    // 칼을 위쪽으로 이동
    pos.setY(
        pos.y() -
        14.0);


    knife_item_->setPos(
        pos);


    // 칼이 원판 위치까지 도착한 경우
    if (pos.y() <=
        knife_target_y_)
    {
        knife_timer_->stop();


        knife_flying_ =
            false;


        knife_item_->setY(
            knife_target_y_);


        // QNode를 이용하여 /knife_throw Topic 전송
        qnode_->publishThrow();


        // 잠시 후 다음 칼을 원래 위치로 이동
        QTimer::singleShot(
            80,
            this,
            [this]()
            {
                if (knife_item_)
                {
                    knife_item_->setY(
                        knife_start_y_);
                }
            });
    }
}


// ==================================================
// 원판 회전 화면 갱신
// ==================================================

void MainWindow::updateVisualRotation()
{
    if (!rotating_group_ ||
        !first_angle_received_)
    {
        return;
    }


    // Logic Node에서 받은 각도와 현재 화면 각도의 차이
    double diff =
        target_disk_angle_ -
        visual_disk_angle_;


    // 0도와 360도 경계에서
    // 반대 방향으로 크게 회전하는 현상 방지
    while (diff > 180.0)
    {
        diff -= 360.0;
    }


    while (diff < -180.0)
    {
        diff += 360.0;
    }


    // 목표 각도로 바로 이동하지 않고 조금씩 따라가도록 해서
    // 화면에서 회전이 부드럽게 보이도록 함
    visual_disk_angle_ +=
        diff *
        0.65;


    if (visual_disk_angle_ >=
        360.0)
    {
        visual_disk_angle_ -=
            360.0;
    }


    if (visual_disk_angle_ <
        0.0)
    {
        visual_disk_angle_ +=
            360.0;
    }


    rotating_group_->setRotation(
        visual_disk_angle_);
}


// ==================================================
// 원판에 꽂힌 칼 표시
// ==================================================

void MainWindow::updateEmbeddedKnives()
{
    // 이전에 그려져 있던 칼 이미지 삭제
    for (QGraphicsPixmapItem *item :
         embedded_knife_items_)
    {
        delete item;
    }


    embedded_knife_items_.clear();


    constexpr double PI =
        3.14159265358979323846;


    // 원판 중심에서 칼이 배치될 거리
    const double radius =
        106.0;


    for (double angle :
         knife_angles_)
    {
        QPixmap pixmap(
            ":/images/knife.png");


        if (pixmap.isNull())
        {
            continue;
        }


        // 원판에 꽂힌 칼은 발사 칼보다 작게 표시
        pixmap =
            pixmap.scaled(
                45,
                115,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);


        QGraphicsPixmapItem *item =
            new QGraphicsPixmapItem(
                pixmap,
                rotating_group_);


        QRectF rect =
            item->boundingRect();


        // 칼이 원판을 중심으로 회전하도록 회전 기준점 설정
        item->setTransformOriginPoint(
            rect.width() / 2.0,
            0.0);


        // 각도를 계산에 사용할 수 있도록 라디안으로 변환
        double rad =
            angle *
            PI /
            180.0;


        // 원판 주변에서 칼이 위치할 X, Y 좌표 계산
        double x =
            -std::sin(rad) *
            radius;


        double y =
            std::cos(rad) *
            radius;


        item->setPos(
            x -
                rect.width() /
                    2.0,
            y);


        item->setRotation(
            angle);


        // 칼날이 원판의 뒤쪽에 들어가 보이도록 설정
        item->setZValue(
            1);


        embedded_knife_items_.push_back(
            item);
    }
}


// ==================================================
// 사과 표시
// ==================================================

void MainWindow::updateAppleTargets()
{
    // 기존 사과 이미지 삭제
    for (QGraphicsPixmapItem *item :
         apple_target_items_)
    {
        delete item;
    }


    apple_target_items_.clear();


    constexpr double PI =
        3.14159265358979323846;


    const double radius =
        145.0;


    for (double angle :
         apple_angles_)
    {
        QPixmap pixmap(
            ":/images/apple_target.png");


        if (pixmap.isNull())
        {
            continue;
        }


        pixmap =
            pixmap.scaled(
                58,
                58,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);


        QGraphicsPixmapItem *item =
            new QGraphicsPixmapItem(
                pixmap,
                rotating_group_);


        QRectF rect =
            item->boundingRect();


        item->setTransformOriginPoint(
            rect.width() / 2.0,
            rect.height() / 2.0);


        double rad =
            angle *
            PI /
            180.0;


        double x =
            -std::sin(rad) *
            radius;


        double y =
            std::cos(rad) *
            radius;


        item->setPos(
            x -
                rect.width() /
                    2.0,
            y -
                rect.height() /
                    2.0);


        // 사과가 원판 바깥 방향을 바라보도록 회전
        item->setRotation(
            angle -
            180.0);


        item->setZValue(
            3);


        apple_target_items_.push_back(
            item);
    }
}


// ==================================================
// 왼쪽 남은 칼 UI 표시
// ==================================================

void MainWindow::updateRemainingKnives()
{
    // Qt Designer에서 만든 칼 QLabel 8개
    std::vector<QLabel *> knife_labels =
    {
        ui_->knifeSlot1,
        ui_->knifeSlot2,
        ui_->knifeSlot3,
        ui_->knifeSlot4,
        ui_->knifeSlot5,
        ui_->knifeSlot6,
        ui_->knifeSlot7,
        ui_->knifeSlot8
    };


    // Designer에서 배치한 Y 위치를 기준으로 정렬
    std::sort(
        knife_labels.begin(),
        knife_labels.end(),
        [](QLabel *a, QLabel *b)
        {
            return a->y() <
                   b->y();
        });


    // 스테이지마다 사용할 칼 개수
    int total_knives =
        4;


    switch (stage_)
    {
    case 1:
        total_knives = 4;
        break;

    case 2:
        total_knives = 5;
        break;

    case 3:
        total_knives = 6;
        break;

    case 4:
        total_knives = 7;
        break;

    case 5:
        total_knives = 8;
        break;

    default:
        total_knives = 4;
        break;
    }


    // 이미 사용한 칼의 개수
    int used_knives =
        total_knives -
        remaining_knives_;


    if (used_knives < 0)
    {
        used_knives = 0;
    }


    if (used_knives >
        total_knives)
    {
        used_knives =
            total_knives;
    }


    QPixmap active(
        ":/images/remaining_knife_active.png");


    QPixmap used(
        ":/images/remaining_knife_used.png");


    // 필요한 칼 개수만 아래쪽부터 표시
    int first_visible =
        8 -
        total_knives;


    for (int i = 0;
         i < 8;
         i++)
    {
        QLabel *label =
            knife_labels[i];


        // 해당 스테이지에서 필요하지 않은 칼은 숨김
        if (i <
            first_visible)
        {
            label->hide();

            continue;
        }


        label->show();


        int index =
            i -
            first_visible;


        // 이미 사용한 칼은 어두운 이미지로 변경
        if (index >=
            total_knives -
            used_knives)
        {
            label->setPixmap(
                used);
        }
        else
        {
            label->setPixmap(
                active);
        }
    }
}


// ==================================================
// 스테이지 UI 갱신
// ==================================================

void MainWindow::updateStageUI()
{
    ui_->stageLabel->setText(
        QString(
            "STAGE %1")
            .arg(stage_));


    QString active =
        "color: rgb(245,181,52);";


    QString inactive =
        "color: rgb(90,100,115);";


    // 현재까지 진행한 스테이지는 활성 색상으로 표시
    ui_->stageDot1->setStyleSheet(
        stage_ >= 1
            ? active
            : inactive);


    ui_->stageDot2->setStyleSheet(
        stage_ >= 2
            ? active
            : inactive);


    ui_->stageDot3->setStyleSheet(
        stage_ >= 3
            ? active
            : inactive);


    ui_->stageDot4->setStyleSheet(
        stage_ >= 4
            ? active
            : inactive);


    ui_->stageDot5->setStyleSheet(
        stage_ >= 5
            ? active
            : inactive);
}


// ==================================================
// GAME OVER 화면
// ==================================================

void MainWindow::showGameOver()
{
    overlay_text_->setPlainText(
        QString(
            "GAME OVER\nSCORE : %1")
            .arg(score_));


    overlay_text_->setDefaultTextColor(
        QColor(
            255,
            80,
            80));


    QRectF rect =
        overlay_text_->boundingRect();


    overlay_text_->setPos(
        (560.0 -
         rect.width()) /
            2.0,
        250.0);


    restart_proxy_->setPos(
        190.0,
        370.0);


    game_overlay_->show();
    overlay_text_->show();
    restart_proxy_->show();
}


// ==================================================
// STAGE CLEAR 화면
// ==================================================

void MainWindow::showStageClear()
{
    overlay_text_->setPlainText(
        QString(
            "STAGE %1\nCLEAR!")
            .arg(stage_));


    overlay_text_->setDefaultTextColor(
        QColor(
            245,
            181,
            52));


    QRectF rect =
        overlay_text_->boundingRect();


    overlay_text_->setPos(
        (560.0 -
         rect.width()) /
            2.0,
        280.0);


    game_overlay_->show();
    overlay_text_->show();


    // Stage Clear는 자동으로 다음 Stage로 넘어가기 때문에
    // Restart 버튼은 표시하지 않음
    restart_proxy_->hide();
}


// ==================================================
// YOU WIN 화면
// ==================================================

void MainWindow::showWin()
{
    overlay_text_->setPlainText(
        QString(
            "YOU WIN!\nSCORE : %1")
            .arg(score_));


    overlay_text_->setDefaultTextColor(
        QColor(
            245,
            181,
            52));


    QRectF rect =
        overlay_text_->boundingRect();


    overlay_text_->setPos(
        (560.0 -
         rect.width()) /
            2.0,
        250.0);


    restart_proxy_->setPos(
        190.0,
        370.0);


    game_overlay_->show();
    overlay_text_->show();
    restart_proxy_->show();
}


// ==================================================
// Overlay 숨기기
// ==================================================

void MainWindow::hideOverlay()
{
    if (game_overlay_)
    {
        game_overlay_->hide();
    }


    if (overlay_text_)
    {
        overlay_text_->hide();
    }


    if (restart_proxy_)
    {
        restart_proxy_->hide();
    }
}


// ==================================================
// 게임 Restart
// ==================================================

void MainWindow::resetGame()
{
    // QNode가 없으면 Reset 불가능
    if (!qnode_)
    {
        return;
    }


    // Logic Node의 Reset Service가 준비되지 않은 경우
    if (!qnode_->resetGame())
    {
        return;
    }


    // 칼 애니메이션 중지
    knife_timer_->stop();


    knife_flying_ =
        false;


    // 발사 칼의 위치 초기화
    if (knife_item_)
    {
        knife_item_->setY(
            knife_start_y_);
    }


    // GUI에 저장된 게임 상태 초기화
    score_ =
        0;


    apple_count_ =
        0;


    stage_ =
        1;


    remaining_knives_ =
        4;


    game_over_ =
        false;


    stage_clear_ =
        false;


    win_ =
        false;


    knife_angles_.clear();
    apple_angles_.clear();


    // 화면의 칼과 사과 초기화
    updateEmbeddedKnives();
    updateAppleTargets();
    updateRemainingKnives();
    updateStageUI();


    // 원판 각도 초기화
    target_disk_angle_ =
        0.0;


    visual_disk_angle_ =
        0.0;


    first_angle_received_ =
        false;


    if (rotating_group_)
    {
        rotating_group_->setRotation(
            0.0);
    }


    // 점수와 사과 개수 초기화
    ui_->scoreLabel->setText(
        "0");


    ui_->appleCountLabel->setText(
        "0");


    hideOverlay();


    setFocus();
}


// ==================================================
// 키보드 입력
// ==================================================

void MainWindow::keyPressEvent(
    QKeyEvent *event)
{
    // Space 키 = 칼 발사
    if (event->key() ==
        Qt::Key_Space)
    {
        throwKnife();

        return;
    }


    // R 키 = 게임 Restart
    if (event->key() ==
        Qt::Key_R)
    {
        resetGame();

        return;
    }


    QMainWindow::keyPressEvent(
        event);
}


// ==================================================
// 창 종료 처리
// ==================================================

void MainWindow::closeEvent(
    QCloseEvent *event)
{
    // 기본 Qt 창 종료 동작 실행
    QMainWindow::closeEvent(
        event);
}


// ==================================================
// Logic Node에서 받은 game_state 문자열 분석
// ==================================================

void MainWindow::parseGameState(
    const QString &state)
{
    // game_state는 ; 기준으로 각각의 값을 구분
    QStringList parts =
        state.split(';');


    std::vector<double>
        new_knives;


    std::vector<double>
        new_apples;


    // 이전 값을 저장하여
    // 실제 값이 변한 경우에만 그래픽을 다시 그림
    int old_stage =
        stage_;


    int old_remaining =
        remaining_knives_;


    std::vector<double> old_knives =
        knife_angles_;


    std::vector<double> old_apples =
        apple_angles_;


    for (const QString &part :
         parts)
    {
        // 원판 각도
        if (part.startsWith(
                "angle="))
        {
            target_disk_angle_ =
                part.mid(6)
                    .toDouble();


            // 처음 받은 각도는 화면 각도와 바로 맞춤
            if (!first_angle_received_)
            {
                visual_disk_angle_ =
                    target_disk_angle_;


                first_angle_received_ =
                    true;
            }
        }


        // 점수
        else if (part.startsWith(
                     "score="))
        {
            score_ =
                part.mid(6)
                    .toInt();
        }


        // 게임 오버 여부
        else if (part.startsWith(
                     "game_over="))
        {
            game_over_ =
                part.mid(10)
                    .toInt() ==
                1;
        }


        // 현재 스테이지
        else if (part.startsWith(
                     "stage="))
        {
            stage_ =
                part.mid(6)
                    .toInt();
        }


        // 남은 칼 개수
        else if (part.startsWith(
                     "remaining="))
        {
            remaining_knives_ =
                part.mid(10)
                    .toInt();
        }


        // Stage Clear 여부
        else if (part.startsWith(
                     "stage_clear="))
        {
            stage_clear_ =
                part.mid(12)
                    .toInt() ==
                1;
        }


        // 최종 승리 여부
        else if (part.startsWith(
                     "win="))
        {
            win_ =
                part.mid(4)
                    .toInt() ==
                1;
        }


        // 획득한 사과 개수
        else if (part.startsWith(
                     "apple_count="))
        {
            apple_count_ =
                part.mid(12)
                    .toInt();
        }


        // 원판에 꽂힌 칼의 각도 목록
        else if (part.startsWith(
                     "knives="))
        {
            QString value =
                part.mid(7);


            if (!value.isEmpty())
            {
                QStringList values =
                    value.split(',');


                for (const QString &number :
                     values)
                {
                    bool ok =
                        false;


                    double angle =
                        number.toDouble(
                            &ok);


                    if (ok)
                    {
                        new_knives.push_back(
                            angle);
                    }
                }
            }
        }


        // 원판에 남아있는 사과의 각도 목록
        else if (part.startsWith(
                     "apples="))
        {
            QString value =
                part.mid(7);


            if (!value.isEmpty())
            {
                QStringList values =
                    value.split(',');


                for (const QString &number :
                     values)
                {
                    bool ok =
                        false;


                    double angle =
                        number.toDouble(
                            &ok);


                    if (ok)
                    {
                        new_apples.push_back(
                            angle);
                    }
                }
            }
        }
    }


    // 새 데이터 저장
    knife_angles_ =
        new_knives;


    apple_angles_ =
        new_apples;


    // 칼 위치가 바뀐 경우 다시 그림
    if (old_knives !=
        knife_angles_)
    {
        updateEmbeddedKnives();
    }


    // 사과 위치가 바뀐 경우 다시 그림
    if (old_apples !=
        apple_angles_)
    {
        updateAppleTargets();
    }


    // 남은 칼 개수가 바뀐 경우 UI 갱신
    if (old_remaining !=
        remaining_knives_)
    {
        updateRemainingKnives();
    }


    // 스테이지가 변경된 경우 UI 갱신
    if (old_stage !=
        stage_)
    {
        updateStageUI();
        updateRemainingKnives();
    }


    // 점수 표시
    ui_->scoreLabel->setText(
        QString::number(
            score_));


    // 획득한 사과 개수 표시
    ui_->appleCountLabel->setText(
        QString::number(
            apple_count_));


    // 현재 게임 상태에 맞는 화면 표시
    if (win_)
    {
        showWin();
    }

    else if (game_over_)
    {
        showGameOver();
    }

    else if (stage_clear_)
    {
        showStageClear();
    }

    else
    {
        hideOverlay();
    }
}