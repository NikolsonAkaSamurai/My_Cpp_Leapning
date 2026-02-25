// SortingManager.cpp
#include "SortingManager.h"
#include <algorithm>

SortingManager::SortingManager(Controller* controller, QObject* parent)
    : QObject(parent), m_controller(controller) {}

QComboBox* SortingManager::createSortComboBox(QWidget* parent) {
    QComboBox* box = new QComboBox(parent);
    box->addItem("Без сортировки", BY_DEFAULT);
    box->addItem("По зарплате (возрастание)", BY_SALARY_ASC);
    box->addItem("По зарплате (убывание)", BY_SALARY_DESC);
    box->addItem("По дате (сначала новые)", BY_DATE_DESC);
    box->addItem("По дате (сначала старые)", BY_DATE_ASC);
    box->addItem("По компании", BY_COMPANY);

    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SortingManager::onSortChanged);
    return box;
}

QList<Vacancy> SortingManager::sortVacancies(const QList<Vacancy>& vacancies, SortType type) {
    QList<Vacancy> sorted = vacancies;

    switch(type) {
        case BY_SALARY_ASC:
            std::sort(sorted.begin(), sorted.end(), [](const Vacancy& a, const Vacancy& b) {
                return a.getAverageSalary() < b.getAverageSalary();
            });
            break;
        case BY_SALARY_DESC:
            std::sort(sorted.begin(), sorted.end(), [](const Vacancy& a, const Vacancy& b) {
                return a.getAverageSalary() > b.getAverageSalary();
            });
            break;
        case BY_DATE_DESC:
            std::sort(sorted.begin(), sorted.end(), [](const Vacancy& a, const Vacancy& b) {
                return a.getPublishedAt() > b.getPublishedAt();
            });
            break;
        case BY_DATE_ASC:
            std::sort(sorted.begin(), sorted.end(), [](const Vacancy& a, const Vacancy& b) {
                return a.getPublishedAt() < b.getPublishedAt();
            });
            break;
        case BY_COMPANY:
            std::sort(sorted.begin(), sorted.end(), [](const Vacancy& a, const Vacancy& b) {
                return a.getCompany() < b.getCompany();
            });
            break;
        default:
            break;
    }
    return sorted;
}

void SortingManager::onSortChanged(int index) {
    QComboBox* box = qobject_cast<QComboBox*>(sender());
    if (!box) return;

    SortType type = (SortType)box->itemData(index).toInt();
    auto* profile = m_controller->getCurrentProfile();
    if (profile) {
        QList<Vacancy> sorted = sortVacancies(profile->vacancies, type);
        emit sortingFinished(sorted);
    }
}