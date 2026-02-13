#pragma once
#include <QWidget>
#include <QPushButton>
#include <QVector>

class PlayerSpace : public QWidget {
    Q_OBJECT
public:
    explicit PlayerSpace(QWidget* parent = nullptr);

private:
    QVector<QPushButton*> m_buttons;

};