#include <QApplication>

#include "knife_hit_game/main_window.hpp"


int main(int argc, char *argv[])
{
    // Qt 프로그램 시작
    QApplication app(
        argc,
        argv);


    // 메인 게임 창 생성
    MainWindow window;


    // 게임 창 표시
    window.show();


    // Qt 이벤트 루프 실행
    return app.exec();
}