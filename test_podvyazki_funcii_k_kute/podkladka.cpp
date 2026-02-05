//
// Created by Николай on 04.02.2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_podkladka.h" resolved

#include "podkladka.h"
#include "ui_podkladka.h"


podkladka::podkladka(QWidget *parent) : QMainWindow(parent), ui(new Ui::podkladka) {
    ui->setupUi(this);
}

podkladka::~podkladka() {
    delete ui;
}