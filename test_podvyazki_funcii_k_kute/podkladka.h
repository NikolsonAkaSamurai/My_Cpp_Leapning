//
// Created by Николай on 04.02.2026.
//

#ifndef TEST_PODVYAZKI_FUNCII_K_KUTE_PODKLADKA_H
#define TEST_PODVYAZKI_FUNCII_K_KUTE_PODKLADKA_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui {
    class podkladka;
}

QT_END_NAMESPACE

class podkladka : public QMainWindow {
    Q_OBJECT

public:
    explicit podkladka(QWidget *parent = nullptr);

    ~podkladka() override;

private:
    Ui::podkladka *ui;
};


#endif //TEST_PODVYAZKI_FUNCII_K_KUTE_PODKLADKA_H