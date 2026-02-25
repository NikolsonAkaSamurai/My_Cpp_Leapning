// ChartManager.h
#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>
#include "../core/Controller.h"

class ChartManager : public QObject {
    Q_OBJECT

public:
    explicit ChartManager(Controller* controller, QObject* parent = nullptr);
    void showSalaryChart(QWidget* parent = nullptr);

private:
    Controller* m_controller;
};

#endif // CHARTMANAGER_H