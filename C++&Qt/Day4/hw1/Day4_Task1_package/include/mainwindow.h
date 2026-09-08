#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // UDP 통신
    QUdpSocket *udpSocket;
    quint16 udpPort = 50000;

    void udpRead();

    // 천지인 키보드
    bool koreanMode;
    bool upperCase;
    bool numberMode;

    QPushButton *lastButton;
    int tapIndex;
    QTimer *multiTapTimer;

    QString committedText;
    QString cho;
    QString jung;
    QString jong;
    QString vowelKeys;

    void updateKeyboard();
    void updateInput();
    void setButtonEffects();

    void changeLanguage();
    void changeShift();
    void changeNumberMode();

    void handleEnglishKey(QPushButton *button);
    void handleNumberKey(QPushButton *button);
    void handleKoreanConsonant(QPushButton *button);
    void handleKoreanVowel(const QString &key);

    QString consonantGroup(QPushButton *button);
    QString vowelFromKeys(const QString &keys) const;
    bool isVowelSequence(const QString &keys) const;

    QString composeCurrent() const;
    void commitCurrent();

    int choIndex(const QString &text) const;
    int jungIndex(const QString &text) const;
    int jongIndex(const QString &text) const;

    void insertMultiTap(const QString &group, QPushButton *button);

    void symbolButtonClicked();
    void hanjaButtonClicked();
    void backspace();
    void addSpace();
    void pressEnter();

    void resetMultiTap();
};

#endif // MAINWINDOW_H
