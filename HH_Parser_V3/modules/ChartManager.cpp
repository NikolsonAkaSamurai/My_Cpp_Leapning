// ChartManager.cpp
#include "ChartManager.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtCharts>
#include <algorithm>



ChartManager::ChartManager(Controller* controller, QObject* parent)
    : QObject(parent), m_controller(controller) {}

void ChartManager::showSalaryChart(QWidget* parent) {
    auto* profile = m_controller->getCurrentProfile();
    if (!profile || profile->vacancies.isEmpty()) {
        QMessageBox::warning(parent, "Ошибка", "Нет данных для графика");
        return;
    }

    QList<int> salaries;
    for (const auto &v : profile->vacancies) {
        if (v.hasSalary()) salaries.append(v.getAverageSalary());
    }

    if (salaries.isEmpty()) {
        QMessageBox::warning(parent, "Ошибка", "Нет вакансий с указанной зарплатой");
        return;
    }

    std::sort(salaries.begin(), salaries.end());

    // Создаем отдельное окно для графика
    QWidget* window = new QWidget(parent, Qt::Window);  // Qt::Window - отдельное окно!
    window->setWindowTitle("График зарплат");
    window->resize(800, 500);  // задаем размер ДО показа

    QVBoxLayout* layout = new QVBoxLayout(window);

    QChart *chart = new QChart();
    chart->setTitle("📊 Распределение зарплат - " + profile->displayName);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarSet *barSet = new QBarSet("Количество вакансий");
    QStringList categories;

    int minSalary = salaries.first() / 20000 * 20000;
    int maxSalary = salaries.last() / 20000 * 20000 + 20000;

    for (int range = minSalary; range < maxSalary; range += 20000) {
        int count = 0;
        for (int salary : salaries) {
            if (salary >= range && salary < range + 20000) count++;
        }
        if (count > 0) {
            *barSet << count;
            categories << QString("%1-%2k").arg(range/1000).arg((range+20000)/1000);
        }
    }

    QBarSeries *series = new QBarSeries();
    series->append(barSet);
    chart->addSeries(series);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Диапазон зарплат (тыс. ₽)");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Количество вакансий");
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(780, 480);

    layout->addWidget(chartView);

    window->show();  // показываем окно
    window->raise();  // поднимаем на передний план
    window->activateWindow();  // активируем
}