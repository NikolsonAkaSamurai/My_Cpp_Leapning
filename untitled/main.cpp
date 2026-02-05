#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QFile>
#include <QSslSocket>
#include <QDebug>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QScrollBar>
#include <QDesktopServices>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QPainter>



// ==================== СТРУКТУРЫ ДАННЫХ ====================
struct ProfileInfo {
    QString name;           // "ЧПУ_05_02_2026_02_57"
    QString displayName;    // "ЧПУ (05.02.2026 02:57)"
    QString folderPath;     // "./profiles/ЧПУ_05_02_2026_02_57/"
    QDateTime created;
    int vacancyCount = 0;
    int avgSalary = 0;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["displayName"] = displayName;
        obj["folderPath"] = folderPath;
        obj["created"] = created.toString(Qt::ISODate);
        obj["vacancyCount"] = vacancyCount;
        obj["avgSalary"] = avgSalary;
        return obj;
    }

    static ProfileInfo fromJson(const QJsonObject &obj) {
        ProfileInfo info;
        info.name = obj["name"].toString();
        info.displayName = obj["displayName"].toString();
        info.folderPath = obj["folderPath"].toString();
        info.created = QDateTime::fromString(obj["created"].toString(), Qt::ISODate);
        info.vacancyCount = obj["vacancyCount"].toInt();
        info.avgSalary = obj["avgSalary"].toInt();
        return info;
    }
};

struct VacancyLink {
    QString id;
    QString url;
    QString name;
    QString published_at;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["url"] = url;
        obj["name"] = name;
        obj["published_at"] = published_at;
        return obj;
    }

    static VacancyLink fromJson(const QJsonObject &obj) {
        VacancyLink link;
        link.id = obj["id"].toString();
        link.url = obj["url"].toString();
        link.name = obj["name"].toString();
        link.published_at = obj["published_at"].toString();
        return link;
    }
};

struct VacancyData {
    QString vacancy_id;
    QString vacancy_name;
    QString company_name;
    QString url;
    QString published_at;
    int salary_from = 0;
    int salary_to = 0;
    QString salary_currency;
    bool salary_gross = false;
    QString address_raw;
    QString experience;
    QString description;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["vacancy_id"] = vacancy_id;
        obj["vacancy_name"] = vacancy_name;
        obj["company_name"] = company_name;
        obj["url"] = url;
        obj["published_at"] = published_at;
        obj["salary_from"] = salary_from;
        obj["salary_to"] = salary_to;
        obj["salary_currency"] = salary_currency;
        obj["salary_gross"] = salary_gross;
        obj["address_raw"] = address_raw;
        obj["experience"] = experience;
        obj["description"] = description;
        return obj;
    }

    static VacancyData fromJson(const QJsonObject &obj) {
        VacancyData v;
        v.vacancy_id = obj["vacancy_id"].toString();
        v.vacancy_name = obj["vacancy_name"].toString();
        v.company_name = obj["company_name"].toString();
        v.url = obj["url"].toString();
        v.published_at = obj["published_at"].toString();
        v.salary_from = obj["salary_from"].toInt();
        v.salary_to = obj["salary_to"].toInt();
        v.salary_currency = obj["salary_currency"].toString();
        v.salary_gross = obj["salary_gross"].toBool();
        v.address_raw = obj["address_raw"].toString();
        v.experience = obj["experience"].toString();
        v.description = obj["description"].toString();
        return v;
    }

    int getAverageSalary() const {
        if (salary_from > 0 && salary_to > 0) {
            return (salary_from + salary_to) / 2;
        } else if (salary_from > 0) {
            return salary_from;
        } else {
            return salary_to;
        }
    }

    bool hasRubleSalary() const {
        return (salary_currency == "RUR" || salary_currency == "RUB") &&
               (salary_from > 0 || salary_to > 0);
    }

    QString getSalaryText() const {
        if (salary_from > 0 && salary_to > 0) {
            return QString("%1 - %2 %3")
                .arg(salary_from).arg(salary_to)
                .arg(salary_currency == "RUR" ? "₽" : salary_currency);
        } else if (salary_from > 0) {
            return QString("от %1 %2")
                .arg(salary_from)
                .arg(salary_currency == "RUR" ? "₽" : salary_currency);
        } else if (salary_to > 0) {
            return QString("до %1 %2")
                .arg(salary_to)
                .arg(salary_currency == "RUR" ? "₽" : salary_currency);
        } else {
            return "з/п не указана";
        }
    }
};

// ==================== ВИДЖЕТ КАРТОЧКИ ПРОФИЛЯ ====================
class ProfileCard : public QFrame {
    Q_OBJECT
public:
    explicit ProfileCard(const ProfileInfo &info, QWidget *parent = nullptr)
        : QFrame(parent), profileInfo(info) {
        setFrameStyle(QFrame::Box | QFrame::Raised);
        setLineWidth(1);
        setFixedSize(250, 80);
        setCursor(Qt::PointingHandCursor);

        setStyleSheet(R"(
            ProfileCard {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #2c3e50, stop:1 #34495e);
                border-radius: 8px;
                padding: 8px;
            }
            ProfileCard:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #3498db, stop:1 #2980b9);
                border: 2px solid #1abc9c;
            }
            QLabel {
                color: white;
                background: transparent;
            }
        )");

        auto *layout = new QVBoxLayout(this);

        // Название профиля
        auto *nameLabel = new QLabel(info.displayName, this);
        nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
        nameLabel->setWordWrap(true);
        layout->addWidget(nameLabel);

        // Статистика
        QString stats = QString("📊 %1 вакансий | 💰 %2 ₽")
            .arg(info.vacancyCount)
            .arg(info.avgSalary);
        auto *statsLabel = new QLabel(stats, this);
        statsLabel->setStyleSheet("font-size: 10px; color: #ecf0f1;");
        layout->addWidget(statsLabel);

        // Дата
        auto *dateLabel = new QLabel(info.created.toString("dd.MM.yyyy hh:mm"), this);
        dateLabel->setStyleSheet("font-size: 9px; color: #bdc3c7;");
        layout->addWidget(dateLabel);
    }

    const ProfileInfo& getInfo() const { return profileInfo; }

signals:
    void clicked(const ProfileInfo &info);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked(profileInfo);
        }
        QFrame::mousePressEvent(event);
    }

    void paintEvent(QPaintEvent *event) override {
        QFrame::paintEvent(event);

        // Рисуем иконку профиля
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(52, 152, 219, 100));
        painter.drawEllipse(10, 10, 20, 20);

        painter.setBrush(Qt::white);
        painter.drawEllipse(15, 15, 10, 10);
    }

private:
    ProfileInfo profileInfo;
};

// ==================== ОСНОВНОЙ ПАРСЕР С ПРОФИЛЯМИ ====================
class HHParser : public QWidget {
    Q_OBJECT

public:
    HHParser(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        networkManager = new QNetworkAccessManager(this);
        loadProfiles();

        // Начальное состояние кнопок
        activateProfileMode();
    }

private:
    void setupUI() {
        setWindowTitle("HH Parser Pro - История запросов");
        resize(1200, 800);

        auto *mainLayout = new QHBoxLayout(this);
        mainLayout->setSpacing(5);

        // ========== ЛЕВАЯ ПАНЕЛЬ: ПРОФИЛИ (80px) ==========
        auto *leftPanel = new QWidget(this);
        leftPanel->setFixedWidth(300);
        leftPanel->setStyleSheet("background-color: #2c3e50;");

        auto *leftLayout = new QVBoxLayout(leftPanel);

        // Заголовок
        auto *titleLabel = new QLabel("📁 ИСТОРИЯ ЗАПРОСОВ", leftPanel);
        titleLabel->setStyleSheet(R"(
            color: white;
            font-weight: bold;
            font-size: 14px;
            padding: 10px;
            background-color: #1a252f;
            border-radius: 5px;
            margin: 5px;
        )");
        titleLabel->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(titleLabel);

        // Прокручиваемая область для профилей
        profilesScroll = new QScrollArea(leftPanel);
        profilesScroll->setWidgetResizable(true);
        profilesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        profilesScroll->setStyleSheet("border: none; background: transparent;");

        profilesContainer = new QWidget(profilesScroll);
        profilesLayout = new QVBoxLayout(profilesContainer);
        profilesLayout->setAlignment(Qt::AlignTop);
        profilesLayout->setSpacing(10);

        profilesScroll->setWidget(profilesContainer);
        leftLayout->addWidget(profilesScroll);

        // Кнопка нового запроса
        btnNewProfile = new QPushButton("➕ НОВЫЙ ЗАПРОС", leftPanel);
        btnNewProfile->setStyleSheet(R"(
            QPushButton {
                background-color: #27ae60;
                color: white;
                font-weight: bold;
                padding: 12px;
                border-radius: 5px;
                margin: 10px;
                border: 2px solid #2ecc71;
            }
            QPushButton:hover {
                background-color: #2ecc71;
            }
        )");
        leftLayout->addWidget(btnNewProfile);

        mainLayout->addWidget(leftPanel);

        // ========== ПРАВАЯ ПАНЕЛЬ: ОСНОВНОЙ ИНТЕРФЕЙС ==========
        auto *rightPanel = new QWidget(this);
        auto *rightLayout = new QVBoxLayout(rightPanel);

        // Кнопки действий
        auto *buttonLayout = new QHBoxLayout();
        btnFindCount = new QPushButton("1. Найти количество", this);
        btnCollectLinks = new QPushButton("2. Собрать ссылки", this);
        btnAnalyze = new QPushButton("3. Анализировать", this);
        btnShowChart = new QPushButton("4. График зарплат", this);
        btnBrowse = new QPushButton("5. Просмотр вакансий", this);
        btnCryptoChart = new QPushButton("6. 📈 Крипто-статистика", this);

        buttonLayout->addWidget(btnFindCount);
        buttonLayout->addWidget(btnCollectLinks);
        buttonLayout->addWidget(btnAnalyze);
        buttonLayout->addWidget(btnShowChart);
        buttonLayout->addWidget(btnBrowse);
        buttonLayout->addWidget(btnCryptoChart);
        rightLayout->addLayout(buttonLayout);

        // Поле поиска
        searchEdit = new QLineEdit("ЧПУ", this);
        rightLayout->addWidget(new QLabel("Поисковый запрос:", this));
        rightLayout->addWidget(searchEdit);

        // Прогресс-бар
        progressBar = new QProgressBar(this);
        progressBar->setVisible(false);
        rightLayout->addWidget(progressBar);

        // Логи
        infoDisplay = new QTextEdit(this);
        infoDisplay->setReadOnly(true);
        infoDisplay->setStyleSheet(R"(
            QTextEdit {
                background-color: #ecf0f1;
                border-radius: 5px;
                padding: 10px;
                font-family: 'Courier New';
            }
        )");
        rightLayout->addWidget(infoDisplay);

        mainLayout->addWidget(rightPanel, 1);

        // Подключения
        connect(btnNewProfile, &QPushButton::clicked, this, &HHParser::createNewProfile);
        connect(btnFindCount, &QPushButton::clicked, this, &HHParser::onFindCountClicked);
        connect(btnCollectLinks, &QPushButton::clicked, this, &HHParser::onCollectLinksClicked);
        connect(btnAnalyze, &QPushButton::clicked, this, &HHParser::onAnalyzeClicked);
        connect(btnShowChart, &QPushButton::clicked, this, &HHParser::onShowChartClicked);
        connect(btnBrowse, &QPushButton::clicked, this, &HHParser::onBrowseClicked);
        connect(btnCryptoChart, &QPushButton::clicked, this, &HHParser::onCryptoChartClicked);
    }

    void log(const QString &text) {
        infoDisplay->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + text);
    }

    QNetworkRequest createRequest(const QUrl &url) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "HH-Parser-Pro/2.0");
        return request;
    }

    // ==================== РАБОТА С ПРОФИЛЯМИ ====================
    void loadProfiles() {
        QDir profilesDir("./profiles");
        if (!profilesDir.exists()) {
            profilesDir.mkpath(".");
            return;
        }

        QFile indexFile("./profiles/_index.json");
        if (!indexFile.open(QIODevice::ReadOnly)) {
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(indexFile.readAll());
        QJsonArray profilesArray = doc.array();

        for (const auto &profile : profilesArray) {
            ProfileInfo info = ProfileInfo::fromJson(profile.toObject());
            profiles.append(info);
            addProfileCard(info);
        }
    }

    void saveProfilesIndex() {
        QJsonArray profilesArray;
        for (const auto &profile : profiles) {
            profilesArray.append(profile.toJson());
        }

        QFile indexFile("./profiles/_index.json");
        if (indexFile.open(QIODevice::WriteOnly)) {
            indexFile.write(QJsonDocument(profilesArray).toJson(QJsonDocument::Indented));
        }
    }

    void addProfileCard(const ProfileInfo &info) {
        auto *card = new ProfileCard(info, profilesContainer);
        profilesLayout->addWidget(card);

        connect(card, &ProfileCard::clicked, this, [this, info]() {
            loadProfile(info);
        });
    }

    void createNewProfile() {
        QString query = searchEdit->text().trimmed();
        if (query.isEmpty()) {
            query = "ЧПУ";
        }

        QString timestamp = QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm");
        QString defaultName = QString("%1_%2").arg(query).arg(timestamp);

        bool ok;
        QString profileName = QInputDialog::getText(this, "Новый запрос",
            "Название профиля:", QLineEdit::Normal, defaultName, &ok);

        if (!ok || profileName.isEmpty()) return;

        // Создаём папку профиля
        QString folderPath = QString("./profiles/%1").arg(profileName);
        QDir().mkpath(folderPath);

        // Создаём информацию о профиле
        ProfileInfo newProfile;
        newProfile.name = profileName;
        newProfile.displayName = QString("%1 (%2)")
            .arg(query)
            .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
        newProfile.folderPath = folderPath;
        newProfile.created = QDateTime::currentDateTime();

        // Добавляем профиль в список
        profiles.append(newProfile);

        // Очищаем временные данные
        analyzedVacancies.clear();
        vacancyLinks.clear();
        totalVacancies = 0;

        // Устанавливаем текущий профиль
        currentProfile = newProfile;

        // Обновляем UI
        addProfileCard(newProfile);
        saveProfilesIndex();
        activateProfileMode();

        log(QString("✅ Создан новый профиль: %1").arg(newProfile.displayName));
        log("➡️ Можно начинать парсинг (кнопки 1-3 активны)");
    }

    void loadProfile(const ProfileInfo &info) {
        qDebug() << "=== Загрузка профиля: " << info.name << " ===";

        currentProfile = info;

        // Загружаем данные профиля, если есть
        QFile vacFile(info.folderPath + "/temp2.json");
        if (vacFile.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(vacFile.readAll());
            QJsonArray vacancies = doc.object()["vacancies"].toArray();

            analyzedVacancies.clear();
            for (const auto &vac : vacancies) {
                analyzedVacancies.append(VacancyData::fromJson(vac.toObject()));
            }
            vacFile.close();

            log(QString("📂 Загружен профиль: %1").arg(info.displayName));
            log(QString("📊 Загружено вакансий: %1").arg(analyzedVacancies.size()));
        } else {
            // Если файла нет - профиль пустой
            analyzedVacancies.clear();
            vacancyLinks.clear();
            totalVacancies = 0;

            log(QString("📂 Загружен профиль: %1 (пустой)").arg(info.displayName));
            log("➡️ Можно начать парсинг (кнопки 1-3 активны)");
        }

        // Обновляем состояние кнопок
        activateProfileMode();
    }

    void activateProfileMode() {
        qDebug() << "=== activateProfileMode() ===";

        // Проверяем, есть ли данные в текущем профиле
        bool hasData = !analyzedVacancies.isEmpty() ||
                       QFile::exists(currentProfile.folderPath + "/temp2.json");

        // Проверяем, новый ли профиль (без данных)
        bool isNewProfile = !hasData && !currentProfile.name.isEmpty();

        qDebug() << "Текущий профиль:" << currentProfile.name;
        qDebug() << "hasData:" << hasData << "isNewProfile:" << isNewProfile;
        qDebug() << "Всего профилей:" << profiles.size();
        qDebug() << "Вакансий в памяти:" << analyzedVacancies.size();

        // === КНОПКИ 4 и 5: РАБОТА С ДАННЫМИ ТЕКУЩЕГО ПРОФИЛЯ ===
        // Активны, если в текущем профиле есть данные
        btnShowChart->setEnabled(hasData);
        btnBrowse->setEnabled(hasData);

        // === КНОПКА 6: КРИПТО-СТАТИСТИКА ===
        // Активна, если есть минимум 2 профиля И в текущем профиле есть данные
        bool canShowCrypto = (profiles.size() >= 2) && hasData;
        btnCryptoChart->setEnabled(canShowCrypto);

        // === КНОПКИ 1-3: ПАРСИНГ ===
        // Активны только для нового профиля
        btnFindCount->setEnabled(isNewProfile);
        // Кнопка 2 активна, если есть найденные вакансии
        btnCollectLinks->setEnabled(isNewProfile && totalVacancies > 0);
        // Кнопка 3 активна, если есть собранные ссылки
        btnAnalyze->setEnabled(isNewProfile && !vacancyLinks.isEmpty());

        // Обновляем стили кнопок
        updateButtonStyles();

        qDebug() << "Состояние кнопок:";
        qDebug() << "  4 (График):" << btnShowChart->isEnabled();
        qDebug() << "  5 (Просмотр):" << btnBrowse->isEnabled();
        qDebug() << "  6 (Крипто):" << btnCryptoChart->isEnabled();
        qDebug() << "  1 (Найти):" << btnFindCount->isEnabled();
        qDebug() << "  2 (Ссылки):" << btnCollectLinks->isEnabled();
        qDebug() << "  3 (Анализ):" << btnAnalyze->isEnabled();
    }

    void updateButtonStyles() {
        QString activeStyle = R"(
            QPushButton {
                background-color: #3498db;
                color: white;
                font-weight: bold;
                padding: 8px;
                border-radius: 4px;
                border: 2px solid #2980b9;
            }
            QPushButton:hover {
                background-color: #2980b9;
                border: 2px solid #1c6ea4;
            }
        )";

        QString disabledStyle = R"(
            QPushButton:disabled {
                background-color: #95a5a6;
                color: #7f8c8d;
                border: 2px solid #7f8c8d;
                padding: 8px;
                border-radius: 4px;
            }
        )";

        // Применяем стили ко всем кнопкам действий
        QPushButton* actionButtons[] = {
            btnFindCount, btnCollectLinks, btnAnalyze,
            btnShowChart, btnBrowse, btnCryptoChart
        };

        for (QPushButton* btn : actionButtons) {
            if (btn) {
                btn->setStyleSheet(btn->isEnabled() ? activeStyle : disabledStyle);
            }
        }
    }

    void updateProfileStats() {
        // Обновляем статистику профиля
        for (auto &profile : profiles) {
            if (profile.name == currentProfile.name) {
                profile.vacancyCount = analyzedVacancies.size();

                // Считаем среднюю зарплату
                long long totalSum = 0;
                int countWithSalary = 0;
                for (const auto &v : analyzedVacancies) {
                    if (v.hasRubleSalary()) {
                        totalSum += v.getAverageSalary();
                        countWithSalary++;
                    }
                }
                profile.avgSalary = countWithSalary > 0 ? totalSum / countWithSalary : 0;

                saveProfilesIndex();
                break;
            }
        }
    }

    // ==================== СОХРАНЕНИЕ ДАННЫХ В ПРОФИЛЬ ====================
    void saveLinksToProfile() {
        QJsonArray linksArray;
        for (const auto &link : vacancyLinks) {
            linksArray.append(link.toJson());
        }

        QJsonObject root;
        root["links"] = linksArray;
        root["total_count"] = vacancyLinks.size();
        root["saved_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        QFile file(currentProfile.folderPath + "/temp1.json");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
            log("✅ Ссылки сохранены в профиль");
        }
    }

    void saveVacanciesToProfile() {
        QJsonArray vacanciesArray;
        for (const auto &v : analyzedVacancies) {
            vacanciesArray.append(v.toJson());
        }

        QJsonObject root;
        root["vacancies"] = vacanciesArray;
        root["total_count"] = analyzedVacancies.size();
        root["saved_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        QFile file(currentProfile.folderPath + "/temp2.json");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
            log("✅ Вакансии сохранены в профиль");

            // Обновляем статистику профиля
            updateProfileStats();
            log("📊 Статистика профиля обновлена");

            // После сохранения данных обновляем состояние кнопок
            activateProfileMode();
        }
    }

private slots:
    void onFindCountClicked() {
        if (currentProfile.name.isEmpty()) {
            log("❌ Сначала создайте профиль (кнопка слева)");
            return;
        }

        QString text = searchEdit->text();
        QUrl url("https://api.hh.ru/vacancies?text=" + text + "&area=2&per_page=1");

        QNetworkReply *reply = networkManager->get(createRequest(url));
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                totalVacancies = doc.object().value("found").toInt();
                totalPages = doc.object().value("pages").toInt();

                log(QString("📊 Найдено вакансий: %1 на %2 страницах.").arg(totalVacancies).arg(totalPages));

                // Активируем кнопку 2, если есть вакансии
                activateProfileMode();
            } else {
                log("❌ Ошибка запроса: " + reply->errorString());
            }
            reply->deleteLater();
        });
    }

    void onCollectLinksClicked() {
        vacancyLinks.clear();
        currentPage = 0;
        progressBar->setVisible(true);
        progressBar->setRange(0, totalPages);
        progressBar->setValue(0);
        startTime = QDateTime::currentMSecsSinceEpoch();

        processNextPage();
    }

    void processNextPage() {
        if (currentPage >= totalPages) {
            log(QString("✅ Сбор ссылок завершен. Собрано: %1").arg(vacancyLinks.size()));
            saveLinksToProfile();
            activateProfileMode();
            progressBar->setVisible(false);
            return;
        }

        QUrl url(QString("https://api.hh.ru/vacancies?text=%1&area=2&per_page=20&page=%2")
                 .arg(searchEdit->text()).arg(currentPage));

        QNetworkReply *reply = networkManager->get(createRequest(url));
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonArray items = doc.object().value("items").toArray();

                if (items.isEmpty()) {
                    log(QString("✅ Страница %1 пустая. Сбор завершен. Всего ссылок: %2")
                        .arg(currentPage).arg(vacancyLinks.size()));
                    saveLinksToProfile();
                    activateProfileMode();
                    progressBar->setVisible(false);
                    reply->deleteLater();
                    return;
                }

                for (const auto &item : items) {
                    QJsonObject vacancyObj = item.toObject();
                    VacancyLink link;
                    link.id = vacancyObj.value("id").toString();
                    link.url = vacancyObj.value("alternate_url").toString();
                    link.name = vacancyObj.value("name").toString();
                    link.published_at = vacancyObj.value("published_at").toString();
                    vacancyLinks.append(link);
                }

                currentPage++;
                progressBar->setValue(currentPage);

                if (currentPage % 5 == 0 || currentPage == totalPages) {
                    log(QString("📄 Страница %1/%2. Ссылок: %3")
                        .arg(currentPage).arg(totalPages).arg(vacancyLinks.size()));
                }

                QTimer::singleShot(1000, this, &HHParser::processNextPage);
            } else {
                log("❌ Ошибка на странице " + QString::number(currentPage));
                QTimer::singleShot(1000, this, &HHParser::processNextPage);
            }
            reply->deleteLater();
        });
    }

    void onAnalyzeClicked() {
        analyzedVacancies.clear();
        currentLinkIndex = 0;
        progressBar->setVisible(true);
        progressBar->setRange(0, vacancyLinks.size());
        progressBar->setValue(0);
        startTime = QDateTime::currentMSecsSinceEpoch();

        processNextVacancy();
    }

    void processNextVacancy() {
        if (currentLinkIndex >= vacancyLinks.size()) {
            finishAnalysis();
            return;
        }

        VacancyLink link = vacancyLinks[currentLinkIndex];
        QUrl url("https://api.hh.ru/vacancies/" + link.id);

        QNetworkReply *reply = networkManager->get(createRequest(url));
        connect(reply, &QNetworkReply::finished, this, [this, reply, link]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject obj = doc.object();

                VacancyData v;
                v.vacancy_id = link.id;
                v.vacancy_name = obj.value("name").toString();
                v.company_name = obj.value("employer").toObject().value("name").toString();
                v.url = link.url;
                v.published_at = obj.value("published_at").toString();

                QJsonObject salary = obj.value("salary").toObject();
                if (!salary.isEmpty()) {
                    v.salary_from = salary.value("from").toInt();
                    v.salary_to = salary.value("to").toInt();
                    v.salary_currency = salary.value("currency").toString();
                    v.salary_gross = salary.value("gross").toBool();
                }

                QJsonValue addressVal = obj.value("address");
                v.address_raw = addressVal.isObject()
                    ? addressVal.toObject().value("raw").toString()
                    : "Нет адреса";

                v.experience = obj.value("experience").toObject().value("name").toString();
                v.description = obj.value("description").toString().left(500);

                analyzedVacancies.append(v);
                currentLinkIndex++;
                progressBar->setValue(currentLinkIndex);

                if (currentLinkIndex % 10 == 0) {
                    log(QString("🔍 Проанализировано: %1/%2")
                        .arg(currentLinkIndex).arg(vacancyLinks.size()));
                }

                QTimer::singleShot(1000, this, &HHParser::processNextVacancy);
            } else {
                log("❌ Ошибка вакансии " + link.id);
                currentLinkIndex++;
                QTimer::singleShot(1000, this, &HHParser::processNextVacancy);
            }
            reply->deleteLater();
        });
    }

    void finishAnalysis() {
        long long totalSum = 0;
        int countWithSalary = 0;

        for (const auto &v : analyzedVacancies) {
            if (v.hasRubleSalary()) {
                totalSum += v.getAverageSalary();
                countWithSalary++;
            }
        }

        log("--- АНАЛИЗ ЗАВЕРШЕН ---");
        log(QString("Всего проанализировано: %1").arg(analyzedVacancies.size()));

        if (countWithSalary > 0) {
            log(QString("💰 Вакансий с указанной ЗП: %1").arg(countWithSalary));
            log(QString("📊 Средняя ЗП: %1 руб.").arg(QString::number(totalSum / countWithSalary, 'f', 0)));
        }

        saveVacanciesToProfile();
        progressBar->setVisible(false);
        activateProfileMode();
    }

    void onShowChartClicked() {
        if (analyzedVacancies.isEmpty()) {
            log("❌ Нет данных для графика");
            QMessageBox::warning(this, "Ошибка", "Нет данных для построения графика!");
            return;
        }

        // Создаём график зарплат
        auto *chart = new QChart();
        chart->setTitle("📊 Распределение зарплат");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Собираем зарплаты
        QList<int> salaries;
        for (const auto &v : analyzedVacancies) {
            if (v.hasRubleSalary()) {
                salaries.append(v.getAverageSalary());
            }
        }

        if (salaries.isEmpty()) {
            log("❌ Нет вакансий с указанной зарплатой в рублях");
            QMessageBox::warning(this, "Ошибка", "Нет данных о зарплатах в рублях!");
            return;
        }

        // Сортируем и группируем
        std::sort(salaries.begin(), salaries.end());

        auto *barSet = new QBarSet("Количество вакансий");
        QStringList categories;

        // Создаём диапазоны (по 10000 руб)
        int minSalary = salaries.first() / 10000 * 10000;
        int maxSalary = salaries.last() / 10000 * 10000 + 10000;

        for (int range = minSalary; range < maxSalary; range += 10000) {
            int count = 0;
            for (int salary : salaries) {
                if (salary >= range && salary < range + 10000) {
                    count++;
                }
            }
            if (count > 0) {
                *barSet << count;
                categories << QString("%1-%2к").arg(range/1000).arg((range+10000)/1000);
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

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        auto *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(800, 600);

        auto *window = new QWidget();
        window->setWindowTitle("График зарплат - " + currentProfile.displayName);
        window->setLayout(new QVBoxLayout());
        window->layout()->addWidget(chartView);
        window->show();

        log("📊 График зарплат построен");
    }

    void onBrowseClicked() {
        if (analyzedVacancies.isEmpty()) {
            log("❌ Нет данных для просмотра");
            QMessageBox::warning(this, "Ошибка", "Нет данных для просмотра!");
            return;
        }

        // Создаём красивое окно просмотра
        auto *browser = new QWidget();
        browser->setWindowTitle("Просмотр вакансий - " + currentProfile.displayName);
        browser->setGeometry(200, 100, 1200, 700);
        browser->setStyleSheet(R"(
            QWidget {
                background-color: #2c3e50;
                color: white;
            }
        )");

        auto *mainLayout = new QHBoxLayout(browser);

        // ========== ЛЕВАЯ ПАНЕЛЬ: СПИСОК ВАКАНСИЙ ==========
        auto *leftPanel = new QWidget(browser);
        leftPanel->setFixedWidth(450);
        leftPanel->setStyleSheet(R"(
            QWidget {
                background-color: #34495e;
                border-radius: 10px;
                padding: 10px;
            }
            QLabel {
                color: white;
                font-size: 12px;
            }
        )");

        auto *leftLayout = new QVBoxLayout(leftPanel);

        // Заголовок
        auto *leftTitle = new QLabel("📋 Вакансии (" + QString::number(analyzedVacancies.size()) + ")", leftPanel);
        leftTitle->setStyleSheet(R"(
            QLabel {
                color: #1abc9c;
                font-size: 18px;
                font-weight: bold;
                padding: 10px;
                border-bottom: 2px solid #1abc9c;
            }
        )");
        leftTitle->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(leftTitle);

        // Прокручиваемая область для карточек
        auto *scrollArea = new QScrollArea(leftPanel);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet(R"(
            QScrollArea {
                border: none;
                background: transparent;
            }
            QScrollBar:vertical {
                background: #2c3e50;
                width: 10px;
                border-radius: 5px;
            }
            QScrollBar::handle:vertical {
                background: #1abc9c;
                border-radius: 5px;
                min-height: 20px;
            }
        )");

        auto *cardsContainer = new QWidget(scrollArea);
        auto *cardsLayout = new QVBoxLayout(cardsContainer);
        cardsLayout->setSpacing(15);
        cardsLayout->setAlignment(Qt::AlignTop);

        scrollArea->setWidget(cardsContainer);
        leftLayout->addWidget(scrollArea);

        mainLayout->addWidget(leftPanel);

        // ========== ПРАВАЯ ПАНЕЛЬ: ДЕТАЛЬНАЯ ИНФОРМАЦИЯ ==========
        auto *rightPanel = new QWidget(browser);
        rightPanel->setStyleSheet(R"(
            QWidget {
                background-color: #34495e;
                border-radius: 10px;
                padding: 15px;
            }
            QTextEdit {
                background-color: #2c3e50;
                color: white;
                border-radius: 8px;
                padding: 15px;
                font-size: 13px;
                border: 1px solid #4a6572;
            }
            QLabel {
                color: white;
            }
        )");

        auto *rightLayout = new QVBoxLayout(rightPanel);

        auto *rightTitle = new QLabel("📄 Детали вакансии", rightPanel);
        rightTitle->setStyleSheet(R"(
            QLabel {
                color: #3498db;
                font-size: 20px;
                font-weight: bold;
                padding: 10px;
                border-bottom: 2px solid #3498db;
            }
        )");
        rightTitle->setAlignment(Qt::AlignCenter);
        rightLayout->addWidget(rightTitle);

        // Детальная информация
        auto *detailsText = new QTextEdit(rightPanel);
        detailsText->setReadOnly(true);
        detailsText->setMinimumHeight(500);
        rightLayout->addWidget(detailsText);

        // Кнопка открытия на сайте
        auto *openButton = new QPushButton("🌐 Открыть на HH.ru", rightPanel);
        openButton->setEnabled(false);
        openButton->setStyleSheet(R"(
            QPushButton {
                background-color: #27ae60;
                color: white;
                font-weight: bold;
                padding: 12px;
                border-radius: 6px;
                border: 2px solid #2ecc71;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #2ecc71;
            }
            QPushButton:disabled {
                background-color: #7f8c8d;
                border: 2px solid #95a5a6;
            }
        )");
        rightLayout->addWidget(openButton);

        mainLayout->addWidget(rightPanel, 1);

        // ========== СОЗДАНИЕ КАРТОЧЕК ВАКАНСИЙ ==========
        QVector<QFrame*> vacancyCards;
        QString *currentVacancyUrl = new QString();

        // Создаём копию данных для использования в лямбдах
        QList<VacancyData> vacanciesCopy = analyzedVacancies;

        for (int i = 0; i < vacanciesCopy.size(); ++i) {
            const auto &v = vacanciesCopy[i];

            // Используем QPushButton вместо QFrame для возможности клика
            auto *cardButton = new QPushButton(cardsContainer);
            cardButton->setFixedHeight(120);
            cardButton->setCursor(Qt::PointingHandCursor);

            // Чередуем цвета карточек
            if (i % 2 == 0) {
                cardButton->setStyleSheet(R"(
                    QPushButton {
                        background-color: #3498db;
                        border-radius: 8px;
                        padding: 12px;
                        border: 2px solid #2980b9;
                        text-align: left;
                    }
                    QPushButton:hover {
                        background-color: #2980b9;
                        border: 2px solid #1abc9c;
                    }
                )");
            } else {
                cardButton->setStyleSheet(R"(
                    QPushButton {
                        background-color: #9b59b6;
                        border-radius: 8px;
                        padding: 12px;
                        border: 2px solid #8e44ad;
                        text-align: left;
                    }
                    QPushButton:hover {
                        background-color: #8e44ad;
                        border: 2px solid #1abc9c;
                    }
                )");
            }

            // Создаём layout внутри кнопки
            auto *buttonLayout = new QVBoxLayout(cardButton);

            // Заголовок вакансии
            auto *titleLabel = new QLabel(v.vacancy_name, cardButton);
            titleLabel->setStyleSheet(R"(
                QLabel {
                    font-weight: bold;
                    font-size: 14px;
                    color: white;
                    background: transparent;
                }
            )");
            titleLabel->setWordWrap(true);
            buttonLayout->addWidget(titleLabel);

            // Компания
            auto *companyLabel = new QLabel("🏢 " + v.company_name, cardButton);
            companyLabel->setStyleSheet("font-size: 12px; color: #ecf0f1; background: transparent;");
            companyLabel->setWordWrap(true);
            buttonLayout->addWidget(companyLabel);

            // Зарплата
            auto *salaryLabel = new QLabel("💰 " + v.getSalaryText(), cardButton);
            salaryLabel->setStyleSheet(R"(
                QLabel {
                    font-weight: bold;
                    font-size: 13px;
                    color: #f1c40f;
                    background-color: rgba(0,0,0,0.2);
                    padding: 3px;
                    border-radius: 4px;
                }
            )");
            buttonLayout->addWidget(salaryLabel);

            // Адрес (если есть)
            if (!v.address_raw.isEmpty() && v.address_raw != "Нет адреса") {
                auto *addressLabel = new QLabel("📍 " + v.address_raw, cardButton);
                addressLabel->setStyleSheet("font-size: 11px; color: #bdc3c7; background: transparent;");
                addressLabel->setWordWrap(true);
                buttonLayout->addWidget(addressLabel);
            }

            cardsLayout->addWidget(cardButton);
            vacancyCards.append((QFrame*)cardButton);  // Приводим к QFrame для совместимости

            // Подключаем клик по карточке - используем захват по значению
            connect(cardButton, &QPushButton::clicked, this, [=]() {
                onVacancyCardSelected(i, detailsText, openButton, currentVacancyUrl,
                                    vacancyCards, scrollArea, cardsContainer, vacanciesCopy);
            });
        }

        // Симулируем клик по первой карточке
        if (!vacanciesCopy.isEmpty()) {
            QTimer::singleShot(100, [=]() {
                onVacancyCardSelected(0, detailsText, openButton, currentVacancyUrl,
                                    vacancyCards, scrollArea, cardsContainer, vacanciesCopy);
            });
        }

        // Подключаем кнопку открытия
        connect(openButton, &QPushButton::clicked, this, [currentVacancyUrl]() {
            if (!currentVacancyUrl->isEmpty()) {
                QDesktopServices::openUrl(QUrl(*currentVacancyUrl));
            }
        });

        // Кнопка закрытия внизу
        auto *closeButton = new QPushButton("✖ Закрыть", browser);
        closeButton->setStyleSheet(R"(
            QPushButton {
                background-color: #e74c3c;
                color: white;
                font-weight: bold;
                padding: 10px;
                border-radius: 5px;
                border: 2px solid #c0392b;
                margin: 10px;
            }
            QPushButton:hover {
                background-color: #c0392b;
            }
        )");
        connect(closeButton, &QPushButton::clicked, browser, &QWidget::close);

        // Добавляем кнопку закрытия под основным layout
        auto *mainWidget = new QWidget();
        auto *finalLayout = new QVBoxLayout(mainWidget);
        finalLayout->addLayout(mainLayout);
        finalLayout->addWidget(closeButton, 0, Qt::AlignCenter);

        auto *finalWindow = new QWidget();
        finalWindow->setLayout(new QVBoxLayout());
        finalWindow->layout()->addWidget(mainWidget);
        finalWindow->setWindowTitle("Просмотр вакансий - " + currentProfile.displayName);
        finalWindow->resize(1200, 750);
        finalWindow->show();

        // Очистка памяти
        connect(browser, &QWidget::destroyed, [currentVacancyUrl]() {
            delete currentVacancyUrl;
        });

        log("📋 Открыт красивый просмотр вакансий");
    }

    // Вспомогательный метод для обработки выбора карточки вакансии
    void onVacancyCardSelected(int index, QTextEdit* detailsText, QPushButton* openButton,
                              QString* currentVacancyUrl, const QVector<QFrame*>& vacancyCards,
                              QScrollArea* scrollArea, QWidget* cardsContainer,
                              const QList<VacancyData>& vacancies) {
        if (index < 0 || index >= vacancies.size()) return;

        const auto &vacancy = vacancies[index];

        // Подсветка выбранной карточки
        for (auto *card : vacancyCards) {
            QString style = card->styleSheet();
            // Убираем предыдущее выделение
            style.replace("border: 3px solid #e74c3c !important;", "");
            card->setStyleSheet(style);
        }

        // Добавляем выделение для выбранной карточки
        if (index < vacancyCards.size()) {
            QString newStyle = vacancyCards[index]->styleSheet();
            if (!newStyle.contains("border: 3px solid #e74c3c !important;")) {
                newStyle += "border: 3px solid #e74c3c !important;";
            }
            vacancyCards[index]->setStyleSheet(newStyle);
        }

        // Обновляем детальную информацию
        QString details = QString(
            "<h2 style='color:#3498db;'>%1</h2>"
            "<h3 style='color:#1abc9c;'>🏢 %2</h3>"
            "<hr style='border:1px solid #4a6572;'>"
            "<table width='100%%' cellpadding='5'>"
            "<tr><td width='30%%'><b>💰 Зарплата:</b></td><td style='color:#f1c40f; font-weight:bold;'>%3</td></tr>"
            "<tr><td><b>📍 Адрес:</b></td><td>%4</td></tr>"
            "<tr><td><b>📅 Опубликована:</b></td><td>%5</td></tr>"
            "<tr><td><b>👨‍💼 Опыт:</b></td><td>%6</td></tr>"
            "</table>"
            "<hr style='border:1px solid #4a6572;'>"
            "<h4 style='color:#3498db;'>📝 Описание:</h4>"
            "<p>%7</p>"
            "<hr style='border:1px solid #4a6572;'>"
            "<p style='color:#95a5a6; font-size:11px;'>ID: %8</p>"
        )
        .arg(vacancy.vacancy_name)
        .arg(vacancy.company_name)
        .arg(vacancy.getSalaryText())
        .arg(vacancy.address_raw.isEmpty() || vacancy.address_raw == "Нет адреса" ? "Не указан" : vacancy.address_raw)
        .arg(vacancy.published_at)
        .arg(vacancy.experience.isEmpty() ? "Не указан" : vacancy.experience)
        .arg(vacancy.description.isEmpty() ?
             "<i style='color:#95a5a6;'>Описание отсутствует</i>" :
             vacancy.description.toHtmlEscaped().replace("\n", "<br>"))
        .arg(vacancy.vacancy_id);

        detailsText->setHtml(details);

        // Активируем кнопку открытия
        *currentVacancyUrl = vacancy.url;
        openButton->setEnabled(!currentVacancyUrl->isEmpty());

        // Прокрутка к выбранной карточке
        if (index < vacancyCards.size()) {
            QScrollBar *scrollBar = scrollArea->verticalScrollBar();
            int cardY = vacancyCards[index]->mapTo(cardsContainer, QPoint(0,0)).y();
            scrollBar->setValue(cardY - 50);
        }
    }

    void onCryptoChartClicked() {
        qDebug() << "Кликнута кнопка 6 (Крипто):";
        qDebug() << "profiles.size() =" << profiles.size();
        qDebug() << "analyzedVacancies.size() =" << analyzedVacancies.size();

        if (profiles.size() < 2) {
            log("❌ Нужно минимум 2 профиля для сравнения");
            QMessageBox::warning(this, "Ошибка", "Создайте минимум 2 профиля для сравнения!");
            return;
        }

        if (analyzedVacancies.isEmpty()) {
            log("❌ Нет данных для построения графика");
            QMessageBox::warning(this, "Ошибка", "В текущем профиле нет данных!");
            return;
        }

        // Создаём крипто-стиль график
        auto *chart = new QChart();
        chart->setTitle("📈 История запросов (крипто-стиль)");
        chart->setTheme(QChart::ChartThemeDark);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Серия 1: Средняя зарплата (зелёная линия)
        auto *salarySeries = new QLineSeries();
        salarySeries->setName("Средняя зарплата");
        salarySeries->setPen(QPen(QColor("#27ae60"), 3));

        // Серия 2: Количество вакансий (оранжевая линия)
        auto *countSeries = new QLineSeries();
        countSeries->setName("Количество вакансий");
        countSeries->setPen(QPen(QColor("#e67e22"), 3));

        QStringList dates;
        for (const auto &profile : profiles) {
            if (profile.avgSalary > 0 && profile.vacancyCount > 0) {
                salarySeries->append(dates.size(), profile.avgSalary / 1000.0); // в тысячах
                countSeries->append(dates.size(), profile.vacancyCount);
                dates << profile.created.toString("dd.MM\nhh:mm");
            }
        }

        if (dates.isEmpty()) {
            log("❌ Нет профилей с данными для сравнения");
            QMessageBox::warning(this, "Ошибка", "Нет профилей с данными для сравнения!");
            return;
        }

        chart->addSeries(salarySeries);
        chart->addSeries(countSeries);

        // Ось X - даты
        auto *axisX = new QBarCategoryAxis();
        axisX->append(dates);
        axisX->setTitleText("Дата запроса");
        chart->addAxis(axisX, Qt::AlignBottom);
        salarySeries->attachAxis(axisX);
        countSeries->attachAxis(axisX);

        // Ось Y слева - зарплата (тыс. руб)
        auto *axisYLeft = new QValueAxis();
        axisYLeft->setTitleText("Зарплата (тыс. ₽)");
        axisYLeft->setTitleBrush(QBrush(QColor("#27ae60")));
        axisYLeft->setLabelsBrush(QBrush(QColor("#27ae60")));
        chart->addAxis(axisYLeft, Qt::AlignLeft);
        salarySeries->attachAxis(axisYLeft);

        // Ось Y справа - количество вакансий
        auto *axisYRight = new QValueAxis();
        axisYRight->setTitleText("Количество вакансий");
        axisYRight->setTitleBrush(QBrush(QColor("#e67e22")));
        axisYRight->setLabelsBrush(QBrush(QColor("#e67e22")));
        chart->addAxis(axisYRight, Qt::AlignRight);
        countSeries->attachAxis(axisYRight);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        auto *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(1000, 600);

        auto *window = new QWidget();
        window->setWindowTitle("📊 Крипто-статистика по профилям");
        window->setLayout(new QVBoxLayout());
        window->layout()->addWidget(chartView);
        window->show();

        log("📈 Крипто-график построен: сравнение всех профилей");
    }

private:
    // UI элементы
    QPushButton *btnNewProfile, *btnFindCount, *btnCollectLinks, *btnAnalyze;
    QPushButton *btnShowChart, *btnBrowse, *btnCryptoChart;
    QLineEdit *searchEdit;
    QTextEdit *infoDisplay;
    QProgressBar *progressBar;
    QNetworkAccessManager *networkManager;

    // Панель профилей
    QScrollArea *profilesScroll;
    QWidget *profilesContainer;
    QVBoxLayout *profilesLayout;

    // Данные
    QList<ProfileInfo> profiles;
    ProfileInfo currentProfile;

    QList<VacancyLink> vacancyLinks;
    QList<VacancyData> analyzedVacancies;

    int totalVacancies = 0;
    int totalPages = 0;
    int currentPage = 0;
    int currentLinkIndex = 0;
    qint64 startTime;
};

int main(int argc, char *argv[]) {
    qputenv("QT_SSL", "schannel");

    QApplication a(argc, argv);

    // Устанавливаем тёмную тему для крипто-стиля
    a.setStyle("Fusion");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    a.setPalette(darkPalette);

    HHParser w;
    w.show();

    return a.exec();
}

#include "main.moc"