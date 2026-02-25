// FilterManager.cpp
#include "FilterManager.h"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>

FilterManager::FilterManager(QObject* parent) : QObject(parent) {
    loadFilters();
}

// ===== СТОП-СЛОВА =====
void FilterManager::addStopWord(const QString& word) {
    if (word.isEmpty()) return;
    m_stopWords.insert(word.toLower());
    saveStopWords();
    emit stopWordsChanged();
}

void FilterManager::removeStopWord(const QString& word) {
    m_stopWords.remove(word.toLower());
    saveStopWords();
    emit stopWordsChanged();
}

QStringList FilterManager::getStopWords() const {
    return m_stopWords.values();
}

bool FilterManager::containsStopWord(const QString& text) const {
    QString lowerText = text.toLower();
    for (const QString& word : m_stopWords) {
        if (lowerText.contains(word)) return true;
    }
    return false;
}

// ===== ЧЕРНЫЙ СПИСОК =====
void FilterManager::addToBlacklist(const QString& vacancyId) {
    m_blacklist.insert(vacancyId);
    saveBlacklist();
    emit blacklistChanged();
}

void FilterManager::removeFromBlacklist(const QString& vacancyId) {
    m_blacklist.remove(vacancyId);
    saveBlacklist();
    emit blacklistChanged();
}

bool FilterManager::isBlacklisted(const QString& vacancyId) const {
    return m_blacklist.contains(vacancyId);
}

QSet<QString> FilterManager::getBlacklist() const {
    return m_blacklist;
}

// ===== ФИЛЬТРАЦИЯ =====
QList<Vacancy> FilterManager::filterVacancies(const QList<Vacancy>& vacancies) const {
    QList<Vacancy> result;
    for (const auto& v : vacancies) {
        if (isBlacklisted(v.getId())) continue;
        if (containsStopWord(v.getName())) continue;
        if (containsStopWord(v.getCompany())) continue;
        result.append(v);
    }
    return result;
}

// ===== UI ДИАЛОГИ =====
void FilterManager::showStopWordsDialog(QWidget* parent) {
    QStringList words = getStopWords();
    QString text = words.isEmpty()
        ? "Стоп-слова отсутствуют"
        : words.join("\n");

    QMessageBox msg(parent);
    msg.setWindowTitle("Управление стоп-словами");
    msg.setText("Текущие стоп-слова:\n" + text);

    QPushButton* addBtn = msg.addButton("Добавить", QMessageBox::ActionRole);
    QPushButton* removeBtn = msg.addButton("Удалить", QMessageBox::ActionRole);
    QPushButton* closeBtn = msg.addButton("Закрыть", QMessageBox::RejectRole);

    msg.exec();

    if (msg.clickedButton() == (QAbstractButton*)addBtn) {
        bool ok;
        QString word = QInputDialog::getText(parent, "Добавить стоп-слово",
                                            "Введите слово:", QLineEdit::Normal,
                                            "", &ok);
        if (ok && !word.isEmpty()) addStopWord(word);
    }
    else if (msg.clickedButton() == (QAbstractButton*)removeBtn) {
        bool ok;
        QString word = QInputDialog::getText(parent, "Удалить стоп-слово",
                                            "Введите слово:", QLineEdit::Normal,
                                            "", &ok);
        if (ok && !word.isEmpty()) removeStopWord(word);
    }
}

// ===== ЗАГРУЗКА/СОХРАНЕНИЕ =====
void FilterManager::loadFilters() {
    loadStopWords();
    loadBlacklist();
}

void FilterManager::loadStopWords() {
    QFile file("data/filters/stopwords.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString word = in.readLine().trimmed().toLower();
            if (!word.isEmpty()) m_stopWords.insert(word);
        }
        file.close();
    }
}

void FilterManager::saveStopWords() {
    QDir().mkpath("data/filters");
    QFile file("data/filters/stopwords.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& word : m_stopWords) {
            out << word << "\n";
        }
        file.close();
    }
}

void FilterManager::loadBlacklist() {
    QFile file("data/filters/blacklist.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> m_blacklist;
        file.close();
    }
}

void FilterManager::saveBlacklist() {
    QDir().mkpath("data/filters");
    QFile file("data/filters/blacklist.dat");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << m_blacklist;
        file.close();
    }
}