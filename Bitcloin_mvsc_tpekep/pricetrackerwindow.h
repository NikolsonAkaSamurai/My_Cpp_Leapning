#ifndef PRICETRACKERWINDOW_H
#define PRICETRACKERWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTimer>

// Подключаем нужные заголовки, чтобы избежать ошибок "неопределенного типа"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

// Предварительные объявления классов
class QTextEdit;
class QLabel;
class QPushButton;
class QChart;



class PriceTrackerWindow : public QMainWindow {
    Q_OBJECT // Обязательный макрос для классов с сигналами и слотами

public:
    explicit PriceTrackerWindow(QWidget *parent = nullptr);
    ~PriceTrackerWindow();

private slots:
    void fetchBitcoinPrice(); // Слот для получения цены
    void onNetworkReply(QNetworkReply *reply); // Слот для обработки ответа сети
    void updateChart();       // Слот для обновления графика
    void startFetching();     // Слот для кнопки "Начать запись"

private:
    void setupUI(); // Метод для создания интерфейса
    void loadLogFile(); // Метод для загрузки содержимого лога в QTextEdit

    // --- Сетевая часть ---
    QNetworkAccessManager *networkManager;
    QTimer *fetchTimer;

    bool isFetchingActive;

    // --- Виджеты интерфейса ---
    QTextEdit *logDisplay;
    QLabel *currentPriceLabel;
    QChartView *chartView;
    QPushButton *startButton;
    QPushButton *updateButton;

    // --- Данные для графика ---
    QLineSeries *priceSeries;
    QChart *mainChart;
};

#endif // PRICETRACKERWINDOW_H
