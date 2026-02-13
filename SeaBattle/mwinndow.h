#pragma once
#include <QMainWindow>
#include "PlayerSpace.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class mWinndow;
}

QT_END_NAMESPACE

class mWinndow : public QMainWindow {
    Q_OBJECT

public:
    explicit mWinndow(QWidget *parent = nullptr);

    ~mWinndow() override;

private:
    Ui::mWinndow *ui;
};
