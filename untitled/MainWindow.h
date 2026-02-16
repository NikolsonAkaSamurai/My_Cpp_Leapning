
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QProgressBar>
#include <QTimer>
#include <QFileDialog>
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QDebug>
#include "controller.h"
#include "hhparser.h"



class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupConnections();
    void updateProfileList();
    void updateVacancyList();
    void updateButtonsState();
    void log(const QString &text);

private slots:
    // Профили
    void onNewProfile();
    void onDeleteProfile();
    void onRenameProfile();
    void onDuplicateProfile();
    void onProfileSelected(QListWidgetItem *item);

    // Поиск
    void onStartSearch();
    void onSearchStarted(const QString &query, int total);
    void onPageParsed(int page, int total, int count);
    void onVacancyParsed(const Vacancy &v);
    void onSearchFinished(const QList<Vacancy> &vacancies);

    // Детали вакансий
    void onVacancySelected(QListWidgetItem *item);
    void onFullVacancyFetched(const Vacancy &v);
    void showVacancyDetails(const Vacancy &v);

    // Графики
    void onShowChart();
    void onShowCryptoChart();
    void onBrowseVacancies();

    // Экспорт
    void onExportJson();
    void onExportCsv();
    void onShowStats();

    // Сигналы контроллера
    void onProfileLoaded(const QString &name, int count);
    void onProfileCreated(const QString &name);
    void onProfileDeleted(const QString &name);
    void onProfileRenamed(const QString &oldName, const QString &newName);

    // Ошибки
    void onParserError(const QString &error);

private:
    // UI элементы
    QListWidget *m_profileList;
    QListWidget *m_vacancyList;
    QLineEdit *m_searchEdit;
    QTextEdit *m_logDisplay;
    QProgressBar *m_progressBar;

    QPushButton *m_btnNew;
    QPushButton *m_btnDelete;
    QPushButton *m_btnRename;
    QPushButton *m_btnDuplicate;
    QPushButton *m_btnSearch;
    QPushButton *m_btnChart;
    QPushButton *m_btnCrypto;
    QPushButton *m_btnBrowse;
    QPushButton *m_btnExportJson;
    QPushButton *m_btnExportCsv;
    QPushButton *m_btnStats;

    // Контроллер и парсер
    Controller &m_controller;
    HHParser *m_parser;
};

#endif // MAINWINDOW_H