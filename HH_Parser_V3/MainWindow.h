// MainWindow.h
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
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QProgressBar>
#include <QTimer>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

#include "core/Controller.h"
#include "core/HHParser.h"
#include "modules/ProfileManager.h"
#include "modules/ChartManager.h"
#include "modules/FilterManager.h"
#include "modules/MapManager.h"
#include "modules/SortingManager.h"
#include "modules/MapWidget.h"  // для доступа к карте
#include <QRegularExpression>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_controller(Controller::instance())
        , m_parser(new HHParser(this))
        , m_profileMgr(new ProfileManager(&m_controller, this))
        , m_chartMgr(new ChartManager(&m_controller, this))
        , m_filterMgr(new FilterManager(this))
        , m_mapMgr(new MapManager(&m_controller, this))
        , m_sortingMgr(new SortingManager(&m_controller, this))
        , m_mapWidget(nullptr)
    {
        setupUI();
        setupConnections();

        m_controller.scanProfiles();
        updateProfileList();

        statusBar()->showMessage("Готов к работе");
    }

private:
    void setupUI() {
        setWindowTitle("HH Parser V3");
        resize(1200, 700);

        auto *central = new QWidget(this);
        auto *mainLayout = new QHBoxLayout(central);

        // ===== ЛЕВАЯ ПАНЕЛЬ (ПРОФИЛИ) =====
        auto *leftPanel = new QWidget();
        leftPanel->setFixedWidth(300);
        leftPanel->setStyleSheet("background-color: #2c3e50;");

        auto *leftLayout = new QVBoxLayout(leftPanel);

        auto *titleLabel = new QLabel("📁 ПРОФИЛИ");
        titleLabel->setStyleSheet("color: white; font-weight: bold; padding: 10px;");
        leftLayout->addWidget(titleLabel);

        m_profileList = new QListWidget();
        m_profileList->setStyleSheet(R"(
            QListWidget {
                background-color: #34495e;
                border: none;
                color: white;
                font-size: 12px;
            }
            QListWidget::item {
                padding: 10px;
                border-bottom: 1px solid #2c3e50;
            }
            QListWidget::item:selected {
                background-color: #3498db;
            }
            QListWidget::item:hover {
                background-color: #2980b9;
            }
        )");
        leftLayout->addWidget(m_profileList);

        // Кнопки профилей
        auto *profileButtonsLayout = new QHBoxLayout();

        m_btnNew = createButton("➕", "#3498db");
        m_btnDelete = createButton("🗑", "#3498db");
        m_btnRename = createButton("✏", "#3498db");
        m_btnDuplicate = createButton("📋", "#3498db");

        profileButtonsLayout->addWidget(m_btnNew);
        profileButtonsLayout->addWidget(m_btnDelete);
        profileButtonsLayout->addWidget(m_btnRename);
        profileButtonsLayout->addWidget(m_btnDuplicate);

        leftLayout->addLayout(profileButtonsLayout);

        // Кнопка управления фильтрами
        m_btnFilters = createButton("🔍 ФИЛЬТРЫ", "#e67e22");
        leftLayout->addWidget(m_btnFilters);

        // ===== ПРАВАЯ ПАНЕЛЬ =====
        auto *rightPanel = new QWidget();
        auto *rightLayout = new QVBoxLayout(rightPanel);

        // Верхняя панель с поиском и кнопками
        auto *topLayout = new QHBoxLayout();

        m_searchEdit = new QLineEdit();
        m_searchEdit->setPlaceholderText("Введите запрос (например: C++ программист)");
        m_searchEdit->setMinimumWidth(300);
        topLayout->addWidget(m_searchEdit);

        m_btnSearch = createActionButton("🔍 ПОИСК");
        m_btnChart = createActionButton("📊 ГРАФИК");
        m_btnMap = createActionButton("🗺 КАРТА");
        m_btnStats = createActionButton("📈 СТАТИСТИКА");

        topLayout->addWidget(m_btnSearch);
        topLayout->addWidget(m_btnChart);
        topLayout->addWidget(m_btnStats);

        // Добавляем сортировку
        topLayout->addWidget(new QLabel("Сортировка:", rightPanel));
        topLayout->addWidget(m_sortingMgr->createSortComboBox(rightPanel));

        m_btnMap = m_mapMgr->createMapButton(this);
        topLayout->addWidget(m_btnMap);

        rightLayout->addLayout(topLayout);

        // Прогресс бар
        m_progressBar = new QProgressBar();
        m_progressBar->setVisible(false);
        rightLayout->addWidget(m_progressBar);

        // Список вакансий
        m_vacancyList = new QListWidget();
        m_vacancyList->setStyleSheet(R"(
            QListWidget {
                border: 1px solid #bdc3c7;
                font-size: 13px;
            }
            QListWidget::item {
                padding: 8px;
                border-bottom: 1px solid #ecf0f1;
            }
            QListWidget::item:selected {
                background-color: #3498db;
                color: white;
            }
        )");
        rightLayout->addWidget(m_vacancyList);

        // Лог
        m_logDisplay = new QTextEdit();
        m_logDisplay->setReadOnly(true);
        m_logDisplay->setMaximumHeight(150);
        m_logDisplay->setStyleSheet("background-color: #ecf0f1; font-family: monospace;");
        rightLayout->addWidget(m_logDisplay);

        mainLayout->addWidget(leftPanel);
        mainLayout->addWidget(rightPanel, 1);

        setCentralWidget(central);

        updateButtonsState();
    }

    QPushButton* createButton(const QString& text, const QString& color) {
        auto* btn = new QPushButton(text);
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background-color: %1;
                color: white;
                font-weight: bold;
                padding: 8px;
                border: none;
                min-width: 40px;
            }
            QPushButton:hover {
                background-color: #2980b9;
            }
        )").arg(color));
        return btn;
    }

    QPushButton* createActionButton(const QString& text) {
        auto* btn = new QPushButton(text);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #27ae60;
                color: white;
                font-weight: bold;
                padding: 8px 15px;
                border: none;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #2ecc71;
            }
            QPushButton:disabled {
                background-color: #95a5a6;
            }
        )");
        return btn;
    }

    void setupConnections() {
        // Кнопки профилей
        connect(m_btnNew, &QPushButton::clicked, this, &MainWindow::onNewProfile);
        connect(m_btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteProfile);
        connect(m_btnRename, &QPushButton::clicked, this, &MainWindow::onRenameProfile);
        connect(m_btnDuplicate, &QPushButton::clicked, this, &MainWindow::onDuplicateProfile);
        connect(m_btnFilters, &QPushButton::clicked, this, &MainWindow::onShowFilters);

        // Выбор профиля
        connect(m_profileList, &QListWidget::itemClicked, this, &MainWindow::onProfileSelected);

        // Кнопки действий
        connect(m_btnSearch, &QPushButton::clicked, this, &MainWindow::onStartSearch);
        connect(m_btnChart, &QPushButton::clicked, this, &MainWindow::onShowChart);
        connect(m_btnMap, &QPushButton::clicked, this, &MainWindow::onShowMap);
        connect(m_btnStats, &QPushButton::clicked, this, &MainWindow::onShowStats);

        // Двойной клик по вакансии
        connect(m_vacancyList, &QListWidget::itemDoubleClicked, this, &MainWindow::onVacancySelected);

        // Сигналы контроллера
        connect(&m_controller, &Controller::profileLoaded, this, &MainWindow::onProfileLoaded);
        connect(&m_controller, &Controller::profileChanged, this, &MainWindow::updateVacancyList);
        connect(&m_controller, &Controller::errorOccurred, this, &MainWindow::onError);
        connect(&m_controller, &Controller::vacancyUpdated, this, &MainWindow::onVacancyUpdated);

        // Сигналы профиль менеджера
        connect(m_profileMgr, &ProfileManager::profileCreated, this, &MainWindow::onProfileCreated);
        connect(m_profileMgr, &ProfileManager::profileDeleted, this, &MainWindow::onProfileDeleted);
        connect(m_profileMgr, &ProfileManager::profileRenamed, this, &MainWindow::onProfileRenamed);

        // Сигналы сортировки
        connect(m_sortingMgr, &SortingManager::sortingFinished, this, &MainWindow::onSortingFinished);

        // Сигналы парсера
        connect(m_parser, &HHParser::searchStarted, this, &MainWindow::onSearchStarted);
        connect(m_parser, &HHParser::pageParsed, this, &MainWindow::onPageParsed);
        connect(m_parser, &HHParser::vacancyParsed, this, &MainWindow::onVacancyParsed);
        connect(m_parser, &HHParser::searchFinished, this, &MainWindow::onSearchFinished);
        connect(m_parser, &HHParser::fullVacancyFetched, this, &MainWindow::onFullVacancyFetched);
        connect(m_parser, &HHParser::errorOccurred, this, &MainWindow::onParserError);
    }

private slots:
    // Профили
    void onNewProfile() {
        QString name = QInputDialog::getText(this, "Новый профиль", "Введите название профиля:");
        if (!name.isEmpty()) m_profileMgr->createProfile(name);
    }

    void onDeleteProfile() {
        auto *item = m_profileList->currentItem();
        if (!item) {
            QMessageBox::warning(this, "Ошибка", "Выберите профиль для удаления");
            return;
        }
        QString name = item->data(Qt::UserRole).toString();
        if (QMessageBox::question(this, "Подтверждение",
            QString("Удалить профиль '%1'?").arg(name),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_profileMgr->deleteProfile(name);
        }
    }

    void onRenameProfile() {
        auto *item = m_profileList->currentItem();
        if (!item) return;
        QString oldName = item->data(Qt::UserRole).toString();
        QString newName = QInputDialog::getText(this, "Переименовать", "Новое название:",
                                               QLineEdit::Normal, oldName);
        if (!newName.isEmpty() && newName != oldName) {
            m_profileMgr->renameProfile(oldName, newName);
        }
    }

    void onDuplicateProfile() {
        auto *item = m_profileList->currentItem();
        if (!item) return;
        QString sourceName = item->data(Qt::UserRole).toString();
        QString newName = QInputDialog::getText(this, "Копировать", "Название копии:");
        if (!newName.isEmpty()) m_profileMgr->duplicateProfile(sourceName, newName);
    }

    void onShowFilters() {
        m_filterMgr->showStopWordsDialog(this);
    }

    void onProfileSelected(QListWidgetItem *item) {
        QString name = item->data(Qt::UserRole).toString();
        m_profileMgr->loadProfile(name);
    }

    // Поиск
    void onStartSearch() {
        if (!m_controller.getCurrentProfile()) {
            QMessageBox::warning(this, "Ошибка", "Выберите профиль");
            return;
        }

        QString query = m_searchEdit->text().trimmed();
        if (query.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите поисковый запрос");
            return;
        }

        m_parser->setArea(2);
        m_parser->setPerPage(20);
        m_parser->setDelay(1000);
        m_parser->searchVacancies(query);
    }

    // Графики
    void onShowChart() {
        m_chartMgr->showSalaryChart(this);
    }

    // Карта
    void onShowMap() {
        m_mapMgr->showMap(this);
        m_mapWidget = m_mapMgr->getMapWidget();  // сохраняем указатель на карту
    }

    // Обновление списка
    void updateProfileList() {
        m_profileList->clear();
        auto profiles = m_controller.getAllProfiles();

        for (const auto &p : profiles) {
            QString display = QString("%1 (%2 вакансий)")
                .arg(p.displayName.isEmpty() ? p.name : p.displayName)
                .arg(p.vacancies.size());

            auto *item = new QListWidgetItem(display);
            item->setData(Qt::UserRole, p.name);
            m_profileList->addItem(item);
        }
    }

    void updateVacancyList() {



        m_vacancyList->clear();
        auto *profile = m_controller.getCurrentProfile();
        if (!profile) return;

        // Применяем фильтры
        QList<Vacancy> filtered = m_filterMgr->filterVacancies(profile->vacancies);

        for (const auto &v : filtered) {
            QString text = QString("%1 | %2 | %3")
                .arg(v.getName())
                .arg(v.getCompany())
                .arg(v.getSalaryText());

            auto *item = new QListWidgetItem(text);
            item->setData(Qt::UserRole, v.getId());
            m_vacancyList->addItem(item);
        }

        // Обновляем карту, если она открыта
        if (m_mapWidget) {
            m_mapWidget->updateVacancies(profile->vacancies);
        }

        qDebug() << "updateVacancyList: m_mapWidget =" << (m_mapWidget ? "est" : "null");

        if (m_mapWidget) {
            qDebug() << "Obnovlaem kaptu s" << profile->vacancies.size() << "vakansiami";
            m_mapWidget->updateVacancies(profile->vacancies);
        }
    }

    void onSortingFinished(const QList<Vacancy>& sorted) {
        m_vacancyList->clear();
        for (const auto &v : sorted) {
            QString text = QString("%1 | %2 | %3")
                .arg(v.getName())
                .arg(v.getCompany())
                .arg(v.getSalaryText());

            auto *item = new QListWidgetItem(text);
            item->setData(Qt::UserRole, v.getId());
            m_vacancyList->addItem(item);
        }
    }

    void updateButtonsState() {
        bool hasProfile = (m_controller.getCurrentProfile() != nullptr);
        m_btnSearch->setEnabled(hasProfile);
        m_btnChart->setEnabled(hasProfile);
        m_btnMap->setEnabled(hasProfile);
        m_btnStats->setEnabled(hasProfile);
    }

    void log(const QString &text) {
        m_logDisplay->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + text);
    }

    // Поиск и парсинг
    void onSearchStarted(const QString &query, int total) {
        log(QString("🔍 Поиск: %1 (найдено: %2)").arg(query).arg(total));
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, total);
        m_progressBar->setValue(0);
        m_btnSearch->setEnabled(false);
        m_vacancyList->clear();
    }

    void onPageParsed(int page, int total, int count) {
        log(QString("📄 Страница %1/%2 | Собрано: %3").arg(page).arg(total).arg(count));
        m_progressBar->setValue(page);
    }

    void onVacancyParsed(const Vacancy &v) {
        m_controller.addVacancyToCurrent(v);
    }

    void onSearchFinished(const QList<Vacancy> &vacancies) {
        m_progressBar->setVisible(false);
        m_btnSearch->setEnabled(true);
        log(QString("✅ Поиск завершен! Загружено: %1").arg(vacancies.size()));
    }

    void onFullVacancyFetched(const Vacancy &v) {
        m_controller.updateVacancyInCurrent(v);

        // 👇 ФИЛЬТРАЦИЯ АДРЕСОВ
        QString address = v.getAddress();
        if (!address.isEmpty() && !v.hasCoords()) {

            // Проверка на минимальную длину
            if (address.length() < 5) {
                log("⚠️ Адрес слишком короткий, геокодирование пропущено: " + address);
            }
            // Проверка на наличие номера дома (хотя бы одной цифры)
            else if (!address.contains(QRegularExpression("[0-9]"))) {
                log("⚠️ Нет номера дома, геокодирование пропущено: " + address);
            }
            else {
                // Если адрес прошёл проверку — ставим в очередь
                Vacancy* vacancyPtr = m_controller.findVacancyInCurrent(v.getId());
                if (vacancyPtr) {
                    m_controller.geocoderWorker()->queueVacancy(v.getId());
                    log("📌 Адрес поставлен в очередь: " + address);
                }
            }
        }

        // Если есть адрес и нет координат — ставим в очередь на геокодирование

        showVacancyDetails(v);
        log("✅ Детали загружены");
    }

    void onVacancySelected(QListWidgetItem *item) {
        QString id = item->data(Qt::UserRole).toString();
        auto *profile = m_controller.getCurrentProfile();
        if (!profile) return;

        for (auto &v : profile->vacancies) {
            if (v.getId() == id) {
                // Подсвечиваем на карте
                if (m_mapWidget) {
                    m_mapWidget->highlightVacancy(id);
                }

                if (!v.hasFullData()) {
                    log("⏳ Загрузка детальной информации...");
                    m_parser->fetchFullVacancy(id);
                } else {
                    showVacancyDetails(v);
                }
                break;
            }
        }
    }

    void onVacancyUpdated(const QString &id) {
        if (m_mapWidget) {
            auto* profile = m_controller.getCurrentProfile();
            if (profile) {
                m_mapWidget->updateVacancies(profile->vacancies);
            }
        }
    }

    void showVacancyDetails(const Vacancy &v) {
        QString details = QString(
            "<h2>%1</h2>"
            "<h3>🏢 %2</h3>"
            "<hr>"
            "<b>💰 Зарплата:</b> %3<br>"
            "<b>📍 Адрес:</b> %4<br>"
            "<b>📅 Опубликована:</b> %5<br>"
            "<b>👨‍💼 Опыт:</b> %6<br>"
            "<b>📝 Занятость:</b> %7<br>"
            "<hr>"
            "<h3>Описание:</h3>"
            "<p>%8</p>"
        ).arg(v.getName())
         .arg(v.getCompany())
         .arg(v.getSalaryText())
         .arg(v.getAddress())
         .arg(v.getPublishedAt())
         .arg(v.getExperience())
         .arg(v.getEmployment() + " / " + v.getSchedule())
         .arg(v.getDescription());

        // Создаем текстовое поле с прокруткой
        QTextEdit *textEdit = new QTextEdit();
        textEdit->setHtml(details);
        textEdit->setReadOnly(true);
        textEdit->setMinimumSize(600, 500);

        // Создаем диалог
        QDialog dialog(this);
        dialog.setWindowTitle("Детали вакансии - " + v.getName());
        dialog.setMinimumSize(650, 550);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->addWidget(textEdit);

        // Кнопка закрытия
        QPushButton *closeBtn = new QPushButton("Закрыть");
        connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
        layout->addWidget(closeBtn, 0, Qt::AlignCenter);

        dialog.exec();
    }

    // Статистика
    void onShowStats() {
        auto *profile = m_controller.getCurrentProfile();
        if (!profile) return;

        int total = profile->vacancies.size();
        int withSalary = m_controller.getVacanciesWithSalary();
        double avgSalary = m_controller.getAverageSalary();

        QString stats = QString(
            "📊 СТАТИСТИКА ПРОФИЛЯ: %1\n"
            "═══════════════════════════\n"
            "Всего вакансий: %2\n"
            "С указанной ЗП: %3\n"
            "Средняя ЗП: %4 ₽\n"
        ).arg(profile->displayName)
         .arg(total)
         .arg(withSalary)
         .arg(QString::number(avgSalary, 'f', 0));

        QMessageBox::information(this, "Статистика", stats);
    }

    // Сигналы профилей
    void onProfileLoaded(const QString &name, int count) {
        statusBar()->showMessage(QString("Загружен профиль: %1 (%2 вакансий)").arg(name).arg(count));
        updateVacancyList();
        updateButtonsState();
        log("✅ Загружен профиль: " + name);
    }

    void onProfileCreated(const QString &name) {
        updateProfileList();
        log("✅ Создан профиль: " + name);
    }

    void onProfileDeleted(const QString &name) {
        updateProfileList();
        if (m_controller.getCurrentProfile() && m_controller.getCurrentProfile()->name == name) {
            m_vacancyList->clear();
            updateButtonsState();
        }
        log("🗑 Удален профиль: " + name);
    }

    void onProfileRenamed(const QString &oldName, const QString &newName) {
        updateProfileList();
        log("✏ Переименован: " + oldName + " → " + newName);
    }

    // Ошибки
    void onParserError(const QString &error) {
        log("❌ Ошибка парсера: " + error);
        m_progressBar->setVisible(false);
        m_btnSearch->setEnabled(true);
    }

    void onError(const QString &error) {
        log("❌ Ошибка: " + error);
    }

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
    QPushButton *m_btnFilters;
    QPushButton *m_btnSearch;
    QPushButton *m_btnChart;
    QPushButton *m_btnMap;
    QPushButton *m_btnStats;

    // Ядро и модули
    Controller &m_controller;
    HHParser *m_parser;
    ProfileManager *m_profileMgr;
    ChartManager *m_chartMgr;
    FilterManager *m_filterMgr;
    MapManager *m_mapMgr;
    SortingManager *m_sortingMgr;
    MapWidget *m_mapWidget;  // указатель на открытую карту
};

#endif // MAINWINDOW_H