//
// Created by Николай on 05.02.2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mWindow.h" resolved

#include "mwindow.h"
#include "ui_mWindow.h"
#include <QPushButton>
#include <QDebug>

mWindow::mWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mWindow) {
    ui->setupUi(this);

        connect(ui->num1, &QPushButton::clicked,
        this, &mWindow::onNum1Clicked);

        connect(ui->num2, &QPushButton::clicked,
            this, &mWindow::onNum2Clicked);

        connect(ui->num3, &QPushButton::clicked,
            this, &mWindow::onNum3Clicked);

        connect(ui->num4, &QPushButton::clicked,
            this, &mWindow::onNum4Clicked);

        connect(ui->num5, &QPushButton::clicked,
            this, &mWindow::onNum5Clicked);

        connect(ui->num6, &QPushButton::clicked,
            this, &mWindow::onNum6Clicked);

        connect(ui->num7, &QPushButton::clicked,
            this, &mWindow::onNum7Clicked);

        connect(ui->num8, &QPushButton::clicked,
            this, &mWindow::onNum8Clicked);

        connect(ui->num9, &QPushButton::clicked,
            this, &mWindow::onNum9Clicked);

        connect(ui->num0, &QPushButton::clicked,
            this, &mWindow::onNum0Clicked);

        connect(ui->sub, &QPushButton::clicked,
            this, &mWindow::onSubClicked);

        connect(ui->add, &QPushButton::clicked,
            this, &mWindow::onAddClicked);

        connect(ui->div, &QPushButton::clicked,
            this, &mWindow::onDivClicked);

        connect(ui->mult, &QPushButton::clicked,
            this, &mWindow::onMultCliked);

        connect(ui->result, &QPushButton::clicked,
            this, &mWindow::onResultClicked);

        connect(ui->clear, &QPushButton::clicked,
            this, &mWindow::clearTablo);

        connect(ui->dec, &QPushButton::clicked,
            this, &mWindow::onDecClicked);
}



mWindow::~mWindow() {
    delete ui;
}

void mWindow::onNum1Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "1");// 6. Добавляем "1" к тексту

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;

}

void mWindow::onNum2Clicked() {

    checkAndClearDisplay();

    qDebug() << "isTypingSecond " << isTypingSecond << "secondMemory "<< secondMemory;

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "2");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();
        qDebug() << "firstNum: " << firstNum;

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
        qDebug() << "secondNum: " << secondNum;
    }

    isTypingSecond = false;
}

void mWindow::onNum3Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "3");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum4Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "4");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum5Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "5");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum6Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "6");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum7Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "7");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum8Clicked() {    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "8");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;}

void mWindow::onNum9Clicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "9");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onNum0Clicked() {
    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "0");

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;
}

void mWindow::onSubClicked() {

    operSymbol = '-';

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "-");

    isTypingSecond = true;

    secondMemory = true;

}

void mWindow::onAddClicked() {

    operSymbol = '+';

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "+");

    isTypingSecond = true;
    secondMemory = true;

}

void mWindow::onMultCliked() {

    operSymbol = '*';

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "*");

    isTypingSecond = true;
    secondMemory = true;
}

void mWindow::onDivClicked() {

    operSymbol = '/';

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + "/");

    isTypingSecond = true;
    secondMemory = true;
}

void mWindow::checkAndClearDisplay() {

    QString text = ui->textEdit->toPlainText();

    if (isTypingSecond) {

        ui->textEdit->clear();
        isTypingSecond = false;

    }
}

void mWindow::onResultClicked() {

    float temp;

    switch (operSymbol) {

        case '+': {
            temp = firstNum + secondNum;
            operSymbol= ' ';
            break;
        }

        case '-': {
            temp = firstNum - secondNum;
            operSymbol= ' ';
            break;
        }
        case '*': {
            temp = firstNum * secondNum;
            operSymbol= ' ';
            break;
        }
        case '/': {
            temp = firstNum / secondNum;
            operSymbol= ' ';
            break;
        }
        default: {
            ui->textEdit->setText("ERROR");
            firstNum = 0.0f;
            secondNum = 0.0f;
            isTypingSecond = true;
            secondMemory = false;
        }
    }

    qDebug() << "fipstNum" << firstNum<< " secondNum " << secondNum << "temp " << temp;

    QString resultString = QString::number(temp);

    ui->textEdit->setText(resultString);

    isTypingSecond = false;

}

void mWindow::clearTablo() {

    ui->textEdit->clear();
    firstNum = 0.0f;
    secondNum = 0.0f;
    isTypingSecond = true;
    secondMemory = false;
}

void mWindow::onDecClicked() {

    checkAndClearDisplay();

    QString currentText = ui->textEdit->toPlainText();

    ui->textEdit->setText(currentText + ".");// добавляем точkу

    if (!secondMemory) {

        firstNum = ui->textEdit->toPlainText().toFloat();

    }else {
        secondNum = ui->textEdit->toPlainText().toFloat();
    }

    isTypingSecond = false;

}