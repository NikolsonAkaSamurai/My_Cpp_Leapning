// mainwindow.cpp
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    m_controller(Controller::instance()),
    m_parser(new HHParser(this))
{
    setupUI();
    setupConnections();

    m_controller.scanProfiles();
    updateProfileList();

    statusBar()->showMessage("Готов к работе");
}

void MainWindow::setupUI() {
    setWindowTitle("HH Parser");
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

    auto *profileButtonsLayout = new QHBoxLayout();

    m_btnNew = new QPushButton("➕");
    m_btnDelete = new QPushButton("🗑");
    m_btnRename = new QPushButton("✏");
    m_btnDuplicate = new QPushButton("📋");

    QString btnStyle = R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            font-weight: bold;
            padding: 8px;
            border: none;
            min-width: 40px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
    )";

    m_btnNew->setStyleSheet(btnStyle);
    m_btnDelete->setStyleSheet(btnStyle);
    m_btnRename->setStyleSheet(btnStyle);
    m_btnDuplicate->setStyleSheet(btnStyle);

    profileButtonsLayout->addWidget(m_btnNew);
    profileButtonsLayout->addWidget(m_btnDelete);
    profileButtonsLayout->addWidget(m_btnRename);
    profileButtonsLayout->addWidget(m_btnDuplicate);

    leftLayout->addLayout(profileButtonsLayout);

    // ===== ПРАВАЯ ПАНЕЛЬ =====
    auto *rightPanel = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightPanel);

    auto *topLayout = new QHBoxLayout();

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Введите запрос (например: C++ программист)");
    m_searchEdit->setMinimumWidth(300);

    m_btnSearch = new QPushButton("🔍 ПОИСК");
    m_btnChart = new QPushButton("📊 ГРАФИК");
    m_btnCrypto = new QPushButton("📈 КРИПТО");
    m_btnBrowse = new QPushButton("👁 ПРОСМОТР");
    m_btnExportJson = new QPushButton("📥 JSON");
    m_btnExportCsv = new QPushButton("📊 CSV");
    m_btnStats = new QPushButton("📈 СТАТИСТИКА");

    QString actionBtnStyle = R"(
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
    )";

    m_btnSearch->setStyleSheet(actionBtnStyle);
    m_btnChart->setStyleSheet(actionBtnStyle);
    m_btnCrypto->setStyleSheet(actionBtnStyle);
    m_btnBrowse->setStyleSheet(actionBtnStyle);
    m_btnExportJson->setStyleSheet(actionBtnStyle);
    m_btnExportCsv->setStyleSheet(actionBtnStyle);
    m_btnStats->setStyleSheet(actionBtnStyle);

    topLayout->addWidget(m_searchEdit);
    topLayout->addWidget(m_btnSearch);
    topLayout->addWidget(m_btnChart);
    topLayout->addWidget(m_btnCrypto);
    topLayout->addWidget(m_btnBrowse);
    topLayout->addWidget(m_btnExportJson);
    topLayout->addWidget(m_btnExportCsv);
    topLayout->addWidget(m_btnStats);

    rightLayout->addLayout(topLayout);

    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    rightLayout->addWidget(m_progressBar);

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

void MainWindow::setupConnections() {
    // Кнопки профилей
    connect(m_btnNew, &QPushButton::clicked, this, &MainWindow::onNewProfile);
    connect(m_btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteProfile);
    connect(m_btnRename, &QPushButton::clicked, this, &MainWindow::onRenameProfile);
    connect(m_btnDuplicate, &QPushButton::clicked, this, &MainWindow::onDuplicateProfile);

    // Выбор профиля
    connect(m_profileList, &QListWidget::itemClicked, this, &MainWindow::onProfileSelected);

    // Кнопки действий
    connect(m_btnSearch, &QPushButton::clicked, this, &MainWindow::onStartSearch);
    connect(m_btnChart, &QPushButton::clicked, this, &MainWindow::onShowChart);
    connect(m_btnCrypto, &QPushButton::clicked, this, &MainWindow::onShowCryptoChart);
    connect(m_btnBrowse, &QPushButton::clicked, this, &MainWindow::onBrowseVacancies);
    connect(m_btnExportJson, &QPushButton::clicked, this, &MainWindow::onExportJson);
    connect(m_btnExportCsv, &QPushButton::clicked, this, &MainWindow::onExportCsv);
    connect(m_btnStats, &QPushButton::clicked, this, &MainWindow::onShowStats);

    // Двойной клик по вакансии
    connect(m_vacancyList, &QListWidget::itemDoubleClicked, this, &MainWindow::onVacancySelected);

    // Сигналы контроллера
    connect(&m_controller, &Controller::profileLoaded, this, &MainWindow::onProfileLoaded);
    connect(&m_controller, &Controller::profileCreated, this, &MainWindow::onProfileCreated);
    connect(&m_controller, &Controller::profileDeleted, this, &MainWindow::onProfileDeleted);
    connect(&m_controller, &Controller::profileRenamed, this, &MainWindow::onProfileRenamed);
    connect(&m_controller, &Controller::profileChanged, this, &MainWindow::updateVacancyList);

    // Сигналы парсера
    connect(m_parser, &HHParser::searchStarted, this, &MainWindow::onSearchStarted);
    connect(m_parser, &HHParser::pageParsed, this, &MainWindow::onPageParsed);
    connect(m_parser, &HHParser::vacancyParsed, this, &MainWindow::onVacancyParsed);
    connect(m_parser, &HHParser::searchFinished, this, &MainWindow::onSearchFinished);
    connect(m_parser, &HHParser::fullVacancyFetched, this, &MainWindow::onFullVacancyFetched);
    connect(m_parser, &HHParser::errorOccurred, this, &MainWindow::onParserError);
}

void MainWindow::updateProfileList() {
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

void MainWindow::updateVacancyList() {
    m_vacancyList->clear();
    auto *profile = m_controller.getCurrentProfile();
    if (!profile) return;

    for (const auto &v : profile->vacancies) {
        QString text = QString("%1 | %2 | %3")
            .arg(v.getName())
            .arg(v.getCompany())
            .arg(v.getSalaryText());

        auto *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, v.getId());
        m_vacancyList->addItem(item);
    }
}

void MainWindow::updateButtonsState() {
    bool hasProfile = (m_controller.getCurrentProfile() != nullptr);
    m_btnSearch->setEnabled(hasProfile);
    m_btnChart->setEnabled(hasProfile);
    m_btnCrypto->setEnabled(hasProfile && m_controller.getAllProfiles().size() > 1);
    m_btnBrowse->setEnabled(hasProfile);
    m_btnExportJson->setEnabled(hasProfile);
    m_btnExportCsv->setEnabled(hasProfile);
    m_btnStats->setEnabled(hasProfile);
}

void MainWindow::log(const QString &text) {
    m_logDisplay->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + text);
}

// ==================== СЛОТЫ ПРОФИЛЕЙ ====================

void MainWindow::onNewProfile() {
    QString name = QInputDialog::getText(this, "Новый профиль", "Введите название профиля:");
    if (!name.isEmpty()) m_controller.createProfile(name);
}

void MainWindow::onDeleteProfile() {
    auto *item = m_profileList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Ошибка", "Выберите профиль для удаления");
        return;
    }

    QString name = item->data(Qt::UserRole).toString();
    if (QMessageBox::question(this, "Подтверждение", QString("Удалить профиль '%1'?").arg(name),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_controller.deleteProfile(name);
    }
}

void MainWindow::onRenameProfile() {
    auto *item = m_profileList->currentItem();
    if (!item) return;

    QString oldName = item->data(Qt::UserRole).toString();
    QString newName = QInputDialog::getText(this, "Переименовать", "Новое название:",
                                           QLineEdit::Normal, oldName);
    if (!newName.isEmpty() && newName != oldName) {
        m_controller.renameProfile(oldName, newName);
    }
}

void MainWindow::onDuplicateProfile() {
    auto *item = m_profileList->currentItem();
    if (!item) return;

    QString sourceName = item->data(Qt::UserRole).toString();
    QString newName = QInputDialog::getText(this, "Копировать", "Название копии:");
    if (!newName.isEmpty()) m_controller.duplicateProfile(sourceName, newName);
}

void MainWindow::onProfileSelected(QListWidgetItem *item) {
    QString name = item->data(Qt::UserRole).toString();
    m_controller.loadProfile(name);
}

void MainWindow::onProfileLoaded(const QString &name, int count) {
    statusBar()->showMessage(QString("Загружен профиль: %1 (%2 вакансий)").arg(name).arg(count));
    updateVacancyList();
    updateButtonsState();
    log("✅ Загружен профиль: " + name);
}

void MainWindow::onProfileCreated(const QString &name) {
    updateProfileList();
    log("✅ Создан профиль: " + name);
}

void MainWindow::onProfileDeleted(const QString &name) {
    updateProfileList();
    if (m_controller.getCurrentProfile() && m_controller.getCurrentProfile()->name == name) {
        m_vacancyList->clear();
        updateButtonsState();
    }
    log("🗑 Удален профиль: " + name);
}

void MainWindow::onProfileRenamed(const QString &oldName, const QString &newName) {
    updateProfileList();
    log("✏ Переименован: " + oldName + " → " + newName);
}

// ==================== СЛОТЫ ПОИСКА ====================

void MainWindow::onStartSearch() {
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

void MainWindow::onSearchStarted(const QString &query, int total) {
    log(QString("🔍 Поиск: %1 (найдено: %2)").arg(query).arg(total));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(0);
    m_btnSearch->setEnabled(false);
    m_vacancyList->clear();
}

void MainWindow::onPageParsed(int page, int total, int count) {
    log(QString("📄 Страница %1/%2 | Собрано: %3").arg(page).arg(total).arg(count));
    m_progressBar->setValue(page);
}

void MainWindow::onVacancyParsed(const Vacancy &v) {
    m_controller.addVacancyToCurrent(v);
}

void MainWindow::onSearchFinished(const QList<Vacancy> &vacancies) {
    m_progressBar->setVisible(false);
    m_btnSearch->setEnabled(true);
    log(QString("✅ Поиск завершен! Загружено: %1").arg(vacancies.size()));
}

// ==================== СЛОТЫ ВАКАНСИЙ ====================

void MainWindow::onVacancySelected(QListWidgetItem *item) {
    QString id = item->data(Qt::UserRole).toString();
    auto *profile = m_controller.getCurrentProfile();
    if (!profile) return;

    for (auto &v : profile->vacancies) {
        if (v.getId() == id) {
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

void MainWindow::onFullVacancyFetched(const Vacancy &v) {
    m_controller.updateVacancyInCurrent(v);
    showVacancyDetails(v);
    log("✅ Детали загружены");
}

void MainWindow::showVacancyDetails(const Vacancy &v) {
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

    QMessageBox::information(this, "Детали вакансии", details);
}

// ==================== ГРАФИКИ ====================

void MainWindow::onShowChart() {
    auto *profile = m_controller.getCurrentProfile();
    if (!profile || profile->vacancies.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для графика");
        return;
    }

    QList<int> salaries;
    for (const auto &v : profile->vacancies) {
        if (v.hasSalary()) salaries.append(v.getAverageSalary());
    }

    if (salaries.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет вакансий с указанной зарплатой");
        return;
    }

    std::sort(salaries.begin(), salaries.end());

    auto *chart = new QChart();
    chart->setTitle("📊 Распределение зарплат - " + profile->displayName);

    auto *barSet = new QBarSet("Количество вакансий");
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

    auto *series = new QBarSeries();
    series->append(barSet);
    chart->addSeries(series);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("Количество вакансий");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(800, 500);

    auto *window = new QWidget();
    window->setWindowTitle("График зарплат");
    window->setLayout(new QVBoxLayout());
    window->layout()->addWidget(view);
    window->show();
}

void MainWindow::onShowCryptoChart() {
    auto profiles = m_controller.getAllProfiles();
    if (profiles.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужно минимум 2 профиля для сравнения");
        return;
    }

    auto *chart = new QChart();
    chart->setTitle("📈 Сравнение профилей (крипто-стиль)");
    chart->setTheme(QChart::ChartThemeDark);

    auto *salarySeries = new QLineSeries();
    salarySeries->setName("Средняя зарплата");
    salarySeries->setPen(QPen(QColor("#27ae60"), 3));

    auto *countSeries = new QLineSeries();
    countSeries->setName("Количество вакансий");
    countSeries->setPen(QPen(QColor("#e67e22"), 3));

    QStringList dates;
    for (const auto &p : profiles) {
        if (p.vacancies.isEmpty()) continue;

        long long total = 0;
        int count = 0;
        for (const auto &v : p.vacancies) {
            if (v.hasSalary()) {
                total += v.getAverageSalary();
                count++;
            }
        }

        if (count > 0) {
            salarySeries->append(dates.size(), total / count / 1000.0);
            countSeries->append(dates.size(), p.vacancies.size());
            dates << p.created.toString("dd.MM");
        }
    }

    if (dates.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет профилей с данными");
        return;
    }

    chart->addSeries(salarySeries);
    chart->addSeries(countSeries);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(dates);
    chart->addAxis(axisX, Qt::AlignBottom);
    salarySeries->attachAxis(axisX);
    countSeries->attachAxis(axisX);

    auto *axisYLeft = new QValueAxis();
    axisYLeft->setTitleText("Зарплата (тыс. ₽)");
    axisYLeft->setTitleBrush(QBrush(QColor("#27ae60")));
    chart->addAxis(axisYLeft, Qt::AlignLeft);
    salarySeries->attachAxis(axisYLeft);

    auto *axisYRight = new QValueAxis();
    axisYRight->setTitleText("Количество");
    axisYRight->setTitleBrush(QBrush(QColor("#e67e22")));
    chart->addAxis(axisYRight, Qt::AlignRight);
    countSeries->attachAxis(axisYRight);

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(900, 500);

    auto *window = new QWidget();
    window->setWindowTitle("Крипто-статистика");
    window->setLayout(new QVBoxLayout());
    window->layout()->addWidget(view);
    window->show();
}

void MainWindow::onBrowseVacancies() {
    auto *profile = m_controller.getCurrentProfile();
    if (!profile || profile->vacancies.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет вакансий для просмотра");
        return;
    }

    auto *browser = new QWidget();
    browser->setWindowTitle("Просмотр вакансий - " + profile->displayName);
    browser->resize(1000, 600);

    auto *layout = new QHBoxLayout(browser);

    auto *listWidget = new QListWidget();
    listWidget->setMinimumWidth(400);

    auto *textEdit = new QTextEdit();
    textEdit->setReadOnly(true);

    for (const auto &v : profile->vacancies) {
        QString itemText = QString("%1 | %2 | %3")
            .arg(v.getName())
            .arg(v.getCompany())
            .arg(v.getSalaryText());
        auto *item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, v.getId());
        listWidget->addItem(item);
    }

    connect(listWidget, &QListWidget::itemClicked, [this, textEdit](QListWidgetItem *item) {
        QString id = item->data(Qt::UserRole).toString();
        auto *profile = m_controller.getCurrentProfile();
        if (!profile) return;

        for (const auto &v : profile->vacancies) {
            if (v.getId() == id) {
                QString details = QString(
                    "<h2>%1</h2>"
                    "<h3>🏢 %2</h3>"
                    "<hr>"
                    "<b>💰 Зарплата:</b> %3<br>"
                    "<b>📍 Адрес:</b> %4<br>"
                    "<b>📅 Опубликована:</b> %5<br>"
                    "<b>👨‍💼 Опыт:</b> %6<br>"
                    "<hr>"
                    "<h3>Описание:</h3>"
                    "%7"
                ).arg(v.getName())
                 .arg(v.getCompany())
                 .arg(v.getSalaryText())
                 .arg(v.getAddress())
                 .arg(v.getPublishedAt())
                 .arg(v.getExperience())
                 .arg(v.getDescription());

                textEdit->setHtml(details);
                break;
            }
        }
    });

    layout->addWidget(listWidget);
    layout->addWidget(textEdit, 1);

    browser->show();
}

// ==================== ЭКСПОРТ ====================

void MainWindow::onExportJson() {
    if (!m_controller.getCurrentProfile()) return;

    QString filename = QFileDialog::getSaveFileName(this, "Сохранить JSON", "", "JSON files (*.json)");
    if (!filename.isEmpty()) {
        if (m_controller.exportToJson(filename)) {
            log("✅ Экспортировано в JSON: " + filename);
        } else {
            log("❌ Ошибка экспорта");
        }
    }
}

void MainWindow::onExportCsv() {
    if (!m_controller.getCurrentProfile()) return;

    QString filename = QFileDialog::getSaveFileName(this, "Сохранить CSV", "", "CSV files (*.csv)");
    if (!filename.isEmpty()) {
        if (m_controller.exportToCsv(filename)) {
            log("✅ Экспортировано в CSV: " + filename);
        } else {
            log("❌ Ошибка экспорта");
        }
    }
}

void MainWindow::onShowStats() {
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

// ==================== ОШИБКИ ====================

void MainWindow::onParserError(const QString &error) {
    log("❌ Ошибка: " + error);
    m_progressBar->setVisible(false);
    m_btnSearch->setEnabled(true);
}