
#include "mwinndow.h"
#include "ui_mWinndow.h"
#include "PlayerSpace.h"


mWinndow::mWinndow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mWinndow) {
    ui->setupUi(this);

    // Создаём PlayerSpace (твой наследник QWidget)
    PlayerSpace* leftPlayer = new PlayerSpace();
    // Вставляем в layout дизайнера
    ui->verticalLayout->addWidget(leftPlayer);  // ← verticalLayout из playerSpace1

    PlayerSpace* rightPlayer = new PlayerSpace();
    ui->verticalLayout_2->addWidget(rightPlayer);  // ← verticalLayout_2 из playerSpace2
}

mWinndow::~mWinndow() {
    delete ui;
}