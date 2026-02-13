#include "PlayerSpace.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

PlayerSpace::PlayerSpace(QWidget* parent) : QWidget(parent) {

    QVBoxLayout* main = new QVBoxLayout(this);
    main->setSpacing(5);              // зазор между элементами
    main->setContentsMargins(0, 0, 0, 0);  // убираем внешние отступы

    // ===== 1. ПОЛЕ ВВОДА ИМЕНИ =====
    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Имя игрока");
    nameEdit->setFixedHeight(35);      // высота
    nameEdit->setFixedWidth(450);      // ширина = ширине поля
    main->addWidget(nameEdit);

    // ===== 2. ПОЛЕ 10x10 =====
    QWidget* field = new QWidget();
    field->setFixedSize(450, 450);     // 44*10 + 1*9 = 440 + 9 = 449? ДА!

    QGridLayout* grid = new QGridLayout(field);
    grid->setHorizontalSpacing(1);
    grid->setVerticalSpacing(1);
    grid->setContentsMargins(0, 0, 0, 0);  // убираем отступы у сетки

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            QPushButton* btn = new QPushButton();
            btn->setFixedSize(44, 44);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            grid->addWidget(btn, i, j);
            m_buttons.append(btn);
        }
    }
    main->addWidget(field);

    // ===== 3. КНОПКА "ГОТОВ" =====
    QPushButton* readyBtn = new QPushButton("Готов");
    readyBtn->setFixedHeight(40);
    readyBtn->setFixedWidth(450);      // ширина = ширине поля
    main->addWidget(readyBtn);

    main->addStretch();  // чтобы всё не прилипало к верху
}