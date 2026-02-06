//
// Created by Николай on 05.02.2026.
//

#ifndef TEST_KUTE_MWINDOW_H
#define TEST_KUTE_MWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui {
    class mWindow;
}

QT_END_NAMESPACE

class mWindow : public QMainWindow {
    Q_OBJECT

private slots:  // ← ДОБАВИТЬ ЭТУ СЕКЦИЮ!
    void onNum1Clicked();  // ← НАШ СЛОТ
    void onNum2Clicked();  // ← НАШ СЛОТ
    void onNum3Clicked();
    void onNum4Clicked();
    void onNum5Clicked();
    void onNum6Clicked();
    void onNum7Clicked();
    void onNum8Clicked();
    void onNum9Clicked();
    void onNum0Clicked();
    void onSubClicked();
    void onAddClicked();
    void onDivClicked();
    void onMultCliked();
    void onResultClicked();
    void checkAndClearDisplay();
    void clearTablo();
    void onDecClicked();


private:
    float firstNum = 0.0f;
    float secondNum = 0.0f;
    char operSymbol= ' ';
    bool isTypingSecond = true;
    bool secondMemory = false;

public:
    explicit mWindow(QWidget *parent = nullptr);

    ~mWindow() override;

private:
    Ui::mWindow *ui;
};


#endif //TEST_KUTE_MWINDOW_H