#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QFont>

// Создаем класс для нашего главного окна
class BtcTrackerWindow : public QWidget {
public:
    // Конструктор окна
    BtcTrackerWindow(QWidget *parent = nullptr) : QWidget(parent) {
        // Настраиваем окно
        setWindowTitle("Bitcoin Price Tracker (Qt)");
        resize(800, 450);

        // Создаем метки для текста
        m_titleLabel = new QLabel("Цена биткоина онлайн", this);
        m_priceLabel = new QLabel("Fetching...", this);

        // Настраиваем шрифты
        QFont titleFont = m_titleLabel->font();
        titleFont.setPointSize(24);
        m_titleLabel->setFont(titleFont);

        QFont priceFont = m_priceLabel->font();
        priceFont.setPointSize(36);
        priceFont.setBold(true);
        m_priceLabel->setFont(priceFont);

        // Создаем вертикальный layout, чтобы красиво расположить метки
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(m_titleLabel);
        layout->addWidget(m_priceLabel);
        layout->setAlignment(Qt::AlignCenter); // Центрируем все

        // Создаем менеджер сетевых запросов
        m_networkManager = new QNetworkAccessManager(this);
        // Соединяем сигнал "запрос завершен" с нашим слотом "обработать ответ"
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &BtcTrackerWindow::onResult);

        // Создаем таймер для периодического обновления
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &BtcTrackerWindow::requestPrice);
        timer->start(10000); // 10000 миллисекунд = 10 секунд

        // Делаем первый запрос при запуске
        requestPrice();
    }

private slots:
    // Слот (функция), который вызывается, когда нужно сделать запрос
    void requestPrice() {
        m_priceLabel->setText("Fetching...");
        m_networkManager->get(QNetworkRequest(QUrl("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd" )));
    }

    // Слот, который вызывается, когда приходит ответ от сервера
    void onResult(QNetworkReply *reply) {
        if (reply->error()) {
            m_priceLabel->setText("Error: " + reply->errorString());
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            double price = obj["bitcoin"].toObject()["usd"].toDouble();
            m_priceLabel->setText("$" + QString::number(price, 'f', 2));
        }
        reply->deleteLater(); // Важно: очищаем память после обработки ответа
    }

private:
    QLabel *m_titleLabel;
    QLabel *m_priceLabel;
    QNetworkAccessManager *m_networkManager;
};


int main(int argc, char *argv[]) {
    // Стандартная "обвязка" для любого приложения Qt
    QApplication app(argc, argv);

    // Создаем и показываем наше окно
    BtcTrackerWindow window;
    window.show();

    // Запускаем главный цикл обработки событий
    return app.exec();
}
