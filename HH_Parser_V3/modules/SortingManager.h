// SortingManager.h
#ifndef SORTINGMANAGER_H
#define SORTINGMANAGER_H

#include <QObject>
#include <QComboBox>
#include "../core/Controller.h"

class SortingManager : public QObject {
    Q_OBJECT

public:
    enum SortType {
        BY_DEFAULT,
        BY_SALARY_ASC,
        BY_SALARY_DESC,
        BY_DATE_ASC,
        BY_DATE_DESC,
        BY_COMPANY
    };

    explicit SortingManager(Controller* controller, QObject* parent = nullptr);
    QComboBox* createSortComboBox(QWidget* parent);
    QList<Vacancy> sortVacancies(const QList<Vacancy>& vacancies, SortType type);

    signals:
        void sortingFinished(const QList<Vacancy>& sorted);

private slots:
    void onSortChanged(int index);

private:
    Controller* m_controller;
};

#endif // SORTINGMANAGER_H