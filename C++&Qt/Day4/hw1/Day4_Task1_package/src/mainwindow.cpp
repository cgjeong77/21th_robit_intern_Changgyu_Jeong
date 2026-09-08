#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLineEdit>
#include <QStringList>
#include <QList>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QMessageBox>

static const QStringList vowelKeyList = {
    "|", "_", ".", "|.", "|..", ".|", "..|", "._", ".._",
    "_.", "_..", "|.|", "|..|", ".||", "..||",
    "._|", "._|.", "._|.|", "_.|", "_..|", "_..||", "_|"
};

static const QStringList vowelList = {
    "ㅣ", "ㅡ", "ㆍ", "ㅏ", "ㅑ", "ㅓ", "ㅕ", "ㅗ", "ㅛ",
    "ㅜ", "ㅠ", "ㅐ", "ㅒ", "ㅔ", "ㅖ",
    "ㅚ", "ㅘ", "ㅙ", "ㅟ", "ㅝ", "ㅞ", "ㅢ"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // UDP 설정
    udpSocket = new QUdpSocket(this);

    if (!udpSocket->bind(QHostAddress::AnyIPv4, udpPort)) {
        QMessageBox::warning(
            this,
            "UDP 오류",
            "9998번 포트를 열 수 없습니다."
            );
    }

    connect(udpSocket, &QUdpSocket::readyRead,
            this, &MainWindow::udpRead);

    koreanMode = true;
    upperCase = false;
    numberMode = false;
    lastButton = nullptr;
    tapIndex = 0;

    multiTapTimer = new QTimer(this);
    multiTapTimer->setSingleShot(true);
    multiTapTimer->setInterval(800);

    connect(multiTapTimer, &QTimer::timeout,
            this, &MainWindow::resetMultiTap);

    QList<QPushButton*> keys = {
        ui->key1Button, ui->key2Button, ui->key3Button,
        ui->key4Button, ui->key5Button, ui->key6Button,
        ui->key7Button, ui->key8Button, ui->key9Button,
        ui->key0Button
    };

    for (int i = 0; i < keys.size(); i++) {
        connect(keys[i], &QPushButton::clicked, this, [this, keys, i]() {
            QPushButton *button = keys[i];

            if (numberMode) {
                handleNumberKey(button);
            }
            else if (koreanMode) {
                if (i == 0)
                    handleKoreanVowel("|");
                else if (i == 1)
                    handleKoreanVowel(".");
                else if (i == 2)
                    handleKoreanVowel("_");
                else
                    handleKoreanConsonant(button);
            }
            else {
                handleEnglishKey(button);
            }
        });
    }

    connect(ui->languageButton, &QPushButton::clicked,
            this, &MainWindow::changeLanguage);

    connect(ui->numberButton, &QPushButton::clicked,
            this, &MainWindow::changeNumberMode);

    connect(ui->backspaceButton, &QPushButton::clicked,
            this, &MainWindow::backspace);

    connect(ui->spaceButton, &QPushButton::clicked,
            this, &MainWindow::addSpace);

    connect(ui->enterButton, &QPushButton::clicked,
            this, &MainWindow::pressEnter);

    connect(ui->inputLineEdit, &QLineEdit::returnPressed,
            this, &MainWindow::pressEnter);

    connect(ui->symbolButton, &QPushButton::clicked,
            this, &MainWindow::symbolButtonClicked);

    connect(ui->hanjaButton, &QPushButton::clicked,
            this, &MainWindow::hanjaButtonClicked);

    setButtonEffects();
    updateKeyboard();
    updateInput();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setButtonEffects()
{
    QList<QPushButton*> buttons =
        ui->keyboardWidget->findChildren<QPushButton*>();

    for (QPushButton *button : buttons) {
        button->setStyleSheet(
            button->styleSheet() +
            "QPushButton:pressed {"
            "background-color:#b9cbe3;"
            "border:2px solid #7d9bc4;"
            "}"
            );
    }
}

void MainWindow::updateKeyboard()
{
    resetMultiTap();

    QList<QPushButton*> keys = {
        ui->key1Button, ui->key2Button, ui->key3Button,
        ui->key4Button, ui->key5Button, ui->key6Button,
        ui->key7Button, ui->key8Button, ui->key9Button,
        ui->key0Button
    };

    QStringList text;

    if (numberMode) {
        text = {
            "1", "2", "3",
            "4", "5", "6",
            "7", "8", "9", "0"
        };

        ui->symbolButton->setText(".,?!");
        ui->hanjaButton->setText("@#%&");

        if (koreanMode)
            ui->numberButton->setText("가나다");
        else
            ui->numberButton->setText("ABC");
    }
    else if (koreanMode) {
        text = {
            "ㅣ", "ㆍ", "ㅡ",
            "ㄱㅋ", "ㄴㄹ", "ㄷㅌ",
            "ㅂㅍ", "ㅅㅎ", "ㅈㅊ", "ㅇㅁ"
        };

        ui->symbolButton->setText(".,?!");
        ui->hanjaButton->setText("한자");
        ui->numberButton->setText("!#1");
    }
    else {
        text = {
            ".,?!", "abc", "def",
            "ghi", "jkl", "mno",
            "pqrs", "tuv", "wxyz", "0"
        };

        if (upperCase) {
            for (QString &s : text)
                s = s.toUpper();
        }

        ui->symbolButton->setText("⇧");
        ui->hanjaButton->setText(",");
        ui->numberButton->setText("!#1");
    }

    for (int i = 0; i < keys.size(); i++)
        keys[i]->setText(text[i]);
}

void MainWindow::changeLanguage()
{
    commitCurrent();

    koreanMode = !koreanMode;
    numberMode = false;

    if (!koreanMode)
        upperCase = false;

    updateKeyboard();
    updateInput();
}

void MainWindow::changeNumberMode()
{
    commitCurrent();

    numberMode = !numberMode;

    updateKeyboard();
    updateInput();
}

void MainWindow::changeShift()
{
    upperCase = !upperCase;
    updateKeyboard();
}

void MainWindow::handleEnglishKey(QPushButton *button)
{
    QList<QPushButton*> keys = {
        ui->key1Button, ui->key2Button, ui->key3Button,
        ui->key4Button, ui->key5Button, ui->key6Button,
        ui->key7Button, ui->key8Button, ui->key9Button,
        ui->key0Button
    };

    QStringList groups = {
        ".,?!", "ABC", "DEF",
        "GHI", "JKL", "MNO",
        "PQRS", "TUV", "WXYZ", "0"
    };

    int index = keys.indexOf(button);

    if (index == -1)
        return;

    QString letters = groups[index];

    if (!upperCase)
        letters = letters.toLower();

    insertMultiTap(letters, button);
}

void MainWindow::handleNumberKey(QPushButton *button)
{
    QList<QPushButton*> keys = {
        ui->key1Button, ui->key2Button, ui->key3Button,
        ui->key4Button, ui->key5Button, ui->key6Button,
        ui->key7Button, ui->key8Button, ui->key9Button,
        ui->key0Button
    };

    int index = keys.indexOf(button);

    if (index == -1)
        return;

    resetMultiTap();
    commitCurrent();

    if (index == 9)
        committedText += "0";
    else
        committedText += QString::number(index + 1);

    updateInput();
}

QString MainWindow::consonantGroup(QPushButton *button)
{
    QList<QPushButton*> keys = {
        ui->key4Button, ui->key5Button, ui->key6Button,
        ui->key7Button, ui->key8Button, ui->key9Button,
        ui->key0Button
    };

    QStringList groups = {
        "ㄱㅋ", "ㄴㄹ", "ㄷㅌ",
        "ㅂㅍ", "ㅅㅎ", "ㅈㅊ", "ㅇㅁ"
    };

    int index = keys.indexOf(button);

    if (index == -1)
        return "";

    return groups[index];
}

void MainWindow::handleKoreanConsonant(QPushButton *button)
{
    QString group = consonantGroup(button);

    if (group.isEmpty())
        return;

    if (button == lastButton && multiTapTimer->isActive()) {
        tapIndex = (tapIndex + 1) % group.length();

        QString c = QString(group[tapIndex]);

        if (!jong.isEmpty())
            jong = c;
        else if (!cho.isEmpty() && jung.isEmpty())
            cho = c;

        updateInput();
        multiTapTimer->start();
        return;
    }

    tapIndex = 0;
    QString c = QString(group[0]);

    if (cho.isEmpty() && jung.isEmpty()) {
        cho = c;
    }
    else if (!cho.isEmpty() && jung.isEmpty()) {
        commitCurrent();
        cho = c;
    }
    else if (cho.isEmpty() && !jung.isEmpty()) {
        commitCurrent();
        cho = c;
    }
    else if (jong.isEmpty()) {
        jong = c;
    }
    else {
        commitCurrent();
        cho = c;
    }

    lastButton = button;
    multiTapTimer->start();

    updateInput();
}

void MainWindow::handleKoreanVowel(const QString &key)
{
    resetMultiTap();

    if (!jong.isEmpty()) {
        QString nextCho = jong;

        jong.clear();
        committedText += composeCurrent();

        cho = nextCho;
        jung.clear();
        vowelKeys.clear();
    }

    if (vowelKeys.isEmpty()) {
        vowelKeys = key;
    }
    else {
        QString next = vowelKeys + key;

        if (isVowelSequence(next)) {
            vowelKeys = next;
        }
        else {
            commitCurrent();
            vowelKeys = key;
        }
    }

    jung = vowelFromKeys(vowelKeys);
    updateInput();
}

QString MainWindow::vowelFromKeys(const QString &keys) const
{
    int index = vowelKeyList.indexOf(keys);

    if (index >= 0)
        return vowelList[index];

    QString text = keys;

    text.replace("|", "ㅣ");
    text.replace("_", "ㅡ");
    text.replace(".", "ㆍ");

    return text;
}

bool MainWindow::isVowelSequence(const QString &keys) const
{
    for (const QString &item : vowelKeyList) {
        if (item.startsWith(keys))
            return true;
    }

    return false;
}

int MainWindow::choIndex(const QString &text) const
{
    static QStringList list = {
        "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ",
        "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ",
        "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
    };

    return list.indexOf(text);
}

int MainWindow::jungIndex(const QString &text) const
{
    static QStringList list = {
        "ㅏ", "ㅐ", "ㅑ", "ㅒ",
        "ㅓ", "ㅔ", "ㅕ", "ㅖ",
        "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ",
        "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ",
        "ㅡ", "ㅢ", "ㅣ"
    };

    return list.indexOf(text);
}

int MainWindow::jongIndex(const QString &text) const
{
    static QStringList list = {
        "",
        "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ",
        "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ",
        "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ",
        "ㅋ", "ㅌ", "ㅍ", "ㅎ"
    };

    return list.indexOf(text);
}

QString MainWindow::composeCurrent() const
{
    if (cho.isEmpty() && jung.isEmpty())
        return "";

    if (cho.isEmpty())
        return jung;

    if (jung.isEmpty())
        return cho;

    int c = choIndex(cho);
    int v = jungIndex(jung);
    int j = jongIndex(jong);

    if (c < 0 || v < 0 || j < 0)
        return cho + jung + jong;

    int code = 0xAC00 + ((c * 21) + v) * 28 + j;

    return QString(QChar(static_cast<ushort>(code)));
}

void MainWindow::commitCurrent()
{
    QString text = composeCurrent();

    if (!text.isEmpty())
        committedText += text;

    cho.clear();
    jung.clear();
    jong.clear();
    vowelKeys.clear();

    updateInput();
}

void MainWindow::insertMultiTap(const QString &group, QPushButton *button)
{
    commitCurrent();

    if (button == lastButton && multiTapTimer->isActive()) {
        tapIndex = (tapIndex + 1) % group.length();

        if (!committedText.isEmpty())
            committedText.chop(1);

        committedText += group[tapIndex];
    }
    else {
        tapIndex = 0;
        committedText += group[0];
    }

    lastButton = button;
    multiTapTimer->start();

    updateInput();
}

void MainWindow::symbolButtonClicked()
{
    if (numberMode) {
        insertMultiTap(".,?!", ui->symbolButton);
    }
    else if (!koreanMode) {
        changeShift();
    }
    else {
        insertMultiTap(".,?!", ui->symbolButton);
    }
}

void MainWindow::hanjaButtonClicked()
{
    if (numberMode) {
        insertMultiTap("@#%&", ui->hanjaButton);
        return;
    }

    if (!koreanMode) {
        resetMultiTap();
        committedText += ",";
        updateInput();
    }
}

void MainWindow::backspace()
{
    resetMultiTap();

    if (!jong.isEmpty()) {
        jong.clear();
    }
    else if (!jung.isEmpty()) {
        if (vowelKeys.length() > 1) {
            vowelKeys.chop(1);
            jung = vowelFromKeys(vowelKeys);
        }
        else {
            jung.clear();
            vowelKeys.clear();
        }
    }
    else if (!cho.isEmpty()) {
        cho.clear();
    }
    else if (!committedText.isEmpty()) {
        committedText.chop(1);
    }

    updateInput();
}

void MainWindow::addSpace()
{
    resetMultiTap();
    commitCurrent();

    committedText += " ";
    updateInput();
}

void MainWindow::pressEnter()
{
    resetMultiTap();
    commitCurrent();

    QString text = ui->inputLineEdit->text();

    if (text.isEmpty())
        return;

    QByteArray data = text.toUtf8();

    QHostAddress friend1("172.100.5.97");

    udpSocket->writeDatagram(data, friend1, udpPort);

    ui->chatTextEdit->append("나 : " + text);

    committedText.clear();
    cho.clear();
    jung.clear();
    jong.clear();
    vowelKeys.clear();

    updateInput();
}

void MainWindow::updateInput()
{
    QString text = committedText + composeCurrent();

    ui->inputLineEdit->setText(text);
    ui->inputLineEdit->setCursorPosition(text.length());
}

void MainWindow::resetMultiTap()
{
    multiTapTimer->stop();
    lastButton = nullptr;
    tapIndex = 0;
}

void MainWindow::udpRead()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram =
            udpSocket->receiveDatagram();

        QString text =
            QString::fromUtf8(datagram.data());

        ui->chatTextEdit->append("상대 : " + text);
    }
}
