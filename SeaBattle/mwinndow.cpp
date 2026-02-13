
#include "mwinndow.h"
#include "ui_mWinndow.h"
#include "PlayerSpace.h"


mWinndow::mWinndow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mWinndow) {
    ui->setupUi(this);

    // Создаём левое поле
    // 1. Создаём поле для левого игрока
    //    и сразу говорим: "твой родитель — playerSpace1"
    PlayerSpace* leftPlayer = new PlayerSpace(ui->playerSpace1);

    // 2. Создаём поле для правого игрока
    //    и сразу говорим: "твой родитель — playerSpace2"
    PlayerSpace* rightPlayer = new PlayerSpace(ui->playerSpace2);
    rightPlayer->setEnabled(false); // вражеское поле выключено
    ui->stackedWidget->setCurrentWidget(ui->startPage);
}

mWinndow::~mWinndow() {
    delete ui;
}