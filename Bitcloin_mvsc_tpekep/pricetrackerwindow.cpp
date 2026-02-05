#include "pricetrackerwindow.h"

// Подключаем все необходимые модули Qt
#include <QApplication>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QPen>
#include <QFont>
#include <QScrollBar>

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

// Модули для графика
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

PriceTrackerWindow::PriceTrackerWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();

    isFetchingActive = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &PriceTrackerWindow::onNetworkReply);

    fetchTimer = new QTimer(this);
    connect(fetchTimer, &QTimer::timeout, this, &PriceTrackerWindow::fetchBitcoinPrice);

    connect(startButton, &QPushButton::clicked, this, &PriceTrackerWindow::startFetching);
    connect(updateButton, &QPushButton::clicked, this, &PriceTrackerWindow::updateChart);

    loadLogFile();
    updateChart();
}

PriceTrackerWindow::~PriceTrackerWindow() {}

void PriceTrackerWindow::setupUI() {
    logDisplay = new QTextEdit;
    logDisplay->setReadOnly(true);
    logDisplay->setFontFamily("Courier");

    currentPriceLabel = new QLabel("Текущая цена: $-----");
    QFont priceFont = currentPriceLabel->font();
    priceFont.setPointSize(16);
    priceFont.setBold(true);
    currentPriceLabel->setFont(priceFont);
    currentPriceLabel->setAlignment(Qt::AlignCenter);

    startButton = new QPushButton("Начать запись");
    updateButton = new QPushButton("Обновить график");

    mainChart = new QChart();
    priceSeries = new QLineSeries();
    mainChart->addSeries(priceSeries);

    mainChart->setTheme(QChart::ChartThemeDark);
    mainChart->legend()->hide();
    QPen seriesPen(QColor(3, 252, 152));
    seriesPen.setWidth(2);
    priceSeries->setPen(seriesPen);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM hh:mm");
    axisX->setLabelsColor(QColor(200, 200, 200));
    mainChart->addAxis(axisX, Qt::AlignBottom);
    priceSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("$%.0f");
    axisY->setLabelsColor(QColor(200, 200, 200));
    mainChart->addAxis(axisY, Qt::AlignLeft);
    priceSeries->attachAxis(axisY);

    chartView = new QChartView(mainChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(chartView);
    rightLayout->addWidget(startButton);
    rightLayout->addWidget(updateButton);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(logDisplay, 1);
    mainLayout->addLayout(rightLayout, 2);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(mainLayout);
    centralLayout->addWidget(currentPriceLabel);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    setWindowTitle("Bitcoin Price Tracker");
    resize(1200, 700);
}

void PriceTrackerWindow::startFetching() {
    if (!isFetchingActive) {
        // --- Логика ВКЛЮЧЕНИЯ записи ---
        fetchTimer->start(60000); // Запрашивать каждые 60 секунд
        startButton->setText("Остановить запись");
        isFetchingActive = true;

        // Делаем первый запрос немедленно, чтобы не ждать 60 секунд
        fetchBitcoinPrice();
        qDebug() << "Запись запущена.";

    } else {
        // --- Логика ВЫКЛЮЧЕНИЯ записи ---
        fetchTimer->stop();
        startButton->setText("Начать запись");
        isFetchingActive = false;
        qDebug() << "Запись остановлена.";
    }
}

void PriceTrackerWindow::fetchBitcoinPrice() {
    qDebug() << "Запрашиваю цену Bitcoin...";
    QNetworkRequest request(QUrl("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd" ));
    networkManager->get(request);
}

void PriceTrackerWindow::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Ошибка сети:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response_data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data);
    double price = jsonDoc.object()["bitcoin"].toObject()["usd"].toDouble();

    if (price > 0) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString logEntry = QString("%1 - Bitcoin Price: $%2").arg(timestamp).arg(price);

        QFile logFile("price_log.txt");
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << logEntry << "\n";
            logFile.close();
        }

        logDisplay->append(logEntry);
        currentPriceLabel->setText(QString("Текущая цена: $%1").arg(price));

        // Сразу обновляем график
        updateChart();
    }
    reply->deleteLater();
}

void PriceTrackerWindow::loadLogFile() {
    QFile logFile("price_log.txt");
    if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logDisplay->setText(logFile.readAll());
        logDisplay->verticalScrollBar()->setValue(logDisplay->verticalScrollBar()->maximum());
    }
}

// ======================================================================
// --- ПОЛНОСТЬЮ ПЕРЕПИСАННЫЙ МЕТОД ---
// ======================================================================
void PriceTrackerWindow::updateChart() {
    qDebug() << "Обновляю график...";

    QFile logFile("price_log.txt");
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QList<QPointF> points;
    QTextStream in(&logFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" - ");
        if (parts.size() < 2) continue;

        QDateTime dateTime = QDateTime::fromString(parts[0], "yyyy-MM-dd hh:mm:ss");
        QString pricePart = parts[1];
        pricePart.remove("Bitcoin Price: $");
        double price = pricePart.toDouble();

        if (dateTime.isValid() && price > 0) {
            points.append(QPointF(dateTime.toMSecsSinceEpoch(), price));
        }
    }
    logFile.close();

    // Заменяем данные в серии. Это более эффективно, чем clear() + append()
    priceSeries->replace(points);

    if (points.isEmpty()) {
        // Если данных нет, ничего не делаем
        return;
    }

    // --- УМНОЕ ОБНОВЛЕНИЕ ОСЕЙ ---
    auto axisX = static_cast<QDateTimeAxis*>(mainChart->axes(Qt::Horizontal).first());
    auto axisY = static_cast<QValueAxis*>(mainChart->axes(Qt::Vertical).first());

    if (points.size() == 1) {
        // --- Обработка случая с ОДНОЙ точкой ---
        qint64 centralTime = points.first().x();
        double centralPrice = points.first().y();

        // Устанавливаем диапазон "вокруг" одной точки, чтобы ее было видно
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(centralTime).addSecs(-60),
                        QDateTime::fromMSecsSinceEpoch(centralTime).addSecs(60));
        axisY->setRange(centralPrice * 0.99, centralPrice * 1.01);

    } else {
        // --- Обработка случая с НЕСКОЛЬКИМИ точками ---
        qint64 minTime = points.first().x();
        qint64 maxTime = points.last().x();

        double minPrice = points.first().y();
        double maxPrice = points.first().y();
        for(const auto& point : points) {
            if (point.y() < minPrice) minPrice = point.y();
            if (point.y() > maxPrice) maxPrice = point.y();
        }

        // Устанавливаем диапазон с небольшими отступами
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(minTime), QDateTime::fromMSecsSinceEpoch(maxTime));
        double margin = (maxPrice - minPrice) * 0.1;
        axisY->setRange(minPrice - margin, maxPrice + margin);
    }
}
