// Vacancy.h
#pragma once

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include "Point.h"  // ← добавляем Point

class Vacancy {
public:
    static const quint32 SERIALIZATION_VERSION = 2;  // увеличили версию!

    // Конструкторы
    Vacancy() : m_salaryFrom(0), m_salaryTo(0), m_hasFullData(false) {}

    Vacancy(const QString &id, const QString &name, const QString &company)
        : m_id(id)
        , m_name(name)
        , m_company(company)
        , m_salaryFrom(0)
        , m_salaryTo(0)
        , m_hasFullData(false)
    {
        m_url = QString("https://hh.ru/vacancy/%1").arg(id);
    }

    Vacancy(const Vacancy &other) = default;
    ~Vacancy() = default;

    // Операторы
    Vacancy& operator=(const Vacancy &other) = default;
    bool operator==(const Vacancy &other) const { return m_id == other.m_id; }
    bool operator!=(const Vacancy &other) const { return !(*this == other); }

    // --- Геттеры ---
    QString getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getCompany() const { return m_company; }
    int getSalaryFrom() const { return m_salaryFrom; }
    int getSalaryTo() const { return m_salaryTo; }
    QString getSalaryCurrency() const { return m_salaryCurrency; }
    QString getUrl() const { return m_url; }
    QString getPublishedAt() const { return m_publishedAt; }
    QString getDescription() const { return m_description; }
    QString getAddress() const { return m_address; }
    QString getExperience() const { return m_experience; }
    QString getEmployment() const { return m_employment; }
    QString getSchedule() const { return m_schedule; }
    bool hasFullData() const { return m_hasFullData; }

    // Новые методы для координат
    Point getLocation() const { return m_location; }
    bool hasCoords() const { return m_location.getX() != 0.0 || m_location.getY() != 0.0; }

    QDateTime getPublishedDateTime() const {
        return QDateTime::fromString(m_publishedAt, Qt::ISODate);
    }

    bool hasSalary() const {
        return (m_salaryFrom > 0 || m_salaryTo > 0) &&
               (m_salaryCurrency == "RUR" || m_salaryCurrency == "RUB");
    }

    int getAverageSalary() const {
        if (!hasSalary()) return 0;
        if (m_salaryFrom > 0 && m_salaryTo > 0) return (m_salaryFrom + m_salaryTo) / 2;
        if (m_salaryFrom > 0) return m_salaryFrom;
        return m_salaryTo;
    }

    QString getSalaryText() const {
        if (!hasSalary()) return "з/п не указана";
        QString currency = (m_salaryCurrency == "RUR" || m_salaryCurrency == "RUB") ? "₽" : m_salaryCurrency;
        if (m_salaryFrom > 0 && m_salaryTo > 0)
            return QString("%1 - %2 %3").arg(m_salaryFrom).arg(m_salaryTo).arg(currency);
        if (m_salaryFrom > 0)
            return QString("от %1 %3").arg(m_salaryFrom).arg(currency);
        return QString("до %1 %3").arg(m_salaryTo).arg(currency);
    }

    // --- Сеттеры ---
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setCompany(const QString &company) { m_company = company; }
    void setSalaryFrom(int salary) { m_salaryFrom = salary; }
    void setSalaryTo(int salary) { m_salaryTo = salary; }
    void setSalaryCurrency(const QString &currency) { m_salaryCurrency = currency; }
    void setUrl(const QString &url) { m_url = url; }
    void setPublishedAt(const QString &publishedAt) { m_publishedAt = publishedAt; }
    void setDescription(const QString &description) { m_description = description; }
    void setAddress(const QString &address) { m_address = address; }
    void setExperience(const QString &experience) { m_experience = experience; }
    void setEmployment(const QString &employment) { m_employment = employment; }
    void setSchedule(const QString &schedule) { m_schedule = schedule; }
    void setFullDataFlag(bool full) { m_hasFullData = full; }

    // Новый сеттер для координат
    void setLocation(const Point& loc) { m_location = loc; }
    void setLocation(double lat, double lon) { m_location = Point(lat, lon); }

    void clearFullData() {
        m_description.clear();
        m_address.clear();
        m_experience.clear();
        m_employment.clear();
        m_schedule.clear();
        m_hasFullData = false;
    }

    // JSON методы
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = m_id;
        obj["name"] = m_name;
        obj["company"] = m_company;
        obj["salary_from"] = m_salaryFrom;
        obj["salary_to"] = m_salaryTo;
        obj["salary_currency"] = m_salaryCurrency;
        obj["url"] = m_url;
        obj["published_at"] = m_publishedAt;
        obj["description"] = m_description;
        obj["address"] = m_address;
        obj["experience"] = m_experience;
        obj["employment"] = m_employment;
        obj["schedule"] = m_schedule;
        obj["has_full_data"] = m_hasFullData;
        if (hasCoords()) {
            obj["lat"] = m_location.getX();
            obj["lon"] = m_location.getY();
        }
        return obj;
    }

    static Vacancy fromJson(const QJsonObject &obj) {
        Vacancy v;
        if (obj.contains("id")) v.setId(obj["id"].toString());
        if (obj.contains("name")) v.setName(obj["name"].toString());
        if (obj.contains("company")) v.setCompany(obj["company"].toString());
        if (obj.contains("salary_from")) v.setSalaryFrom(obj["salary_from"].toInt());
        if (obj.contains("salary_to")) v.setSalaryTo(obj["salary_to"].toInt());
        if (obj.contains("salary_currency")) v.setSalaryCurrency(obj["salary_currency"].toString());
        if (obj.contains("url")) v.setUrl(obj["url"].toString());
        if (obj.contains("published_at")) v.setPublishedAt(obj["published_at"].toString());
        if (obj.contains("description")) v.setDescription(obj["description"].toString());
        if (obj.contains("address")) v.setAddress(obj["address"].toString());
        if (obj.contains("experience")) v.setExperience(obj["experience"].toString());
        if (obj.contains("employment")) v.setEmployment(obj["employment"].toString());
        if (obj.contains("schedule")) v.setSchedule(obj["schedule"].toString());
        if (obj.contains("has_full_data")) v.setFullDataFlag(obj["has_full_data"].toBool());
        if (obj.contains("lat") && obj.contains("lon")) {
            v.setLocation(obj["lat"].toDouble(), obj["lon"].toDouble());
        }
        return v;
    }

    // Сериализация
    void serialize(QDataStream &out) const {
        out << SERIALIZATION_VERSION
            << m_id << m_name << m_company
            << m_salaryFrom << m_salaryTo << m_salaryCurrency
            << m_url << m_publishedAt
            << m_description << m_address << m_experience
            << m_employment << m_schedule
            << m_hasFullData
            << m_location.getX() << m_location.getY();  // новые поля
    }

    void deserialize(QDataStream &in) {
        quint32 version;
        in >> version;
        if (version == 1) {
            in >> m_id >> m_name >> m_company
               >> m_salaryFrom >> m_salaryTo >> m_salaryCurrency
               >> m_url >> m_publishedAt
               >> m_description >> m_address >> m_experience
               >> m_employment >> m_schedule
               >> m_hasFullData;
            m_location = Point(0, 0);  // старые файлы без координат
        } else if (version == 2) {
            in >> m_id >> m_name >> m_company
               >> m_salaryFrom >> m_salaryTo >> m_salaryCurrency
               >> m_url >> m_publishedAt
               >> m_description >> m_address >> m_experience
               >> m_employment >> m_schedule
               >> m_hasFullData;
            double lat, lon;
            in >> lat >> lon;
            m_location = Point(lat, lon);
        }
    }

    // Для отладки
    void print() const { qDebug().noquote() << toString(); }

    QString toString() const {
        QString result;
        QTextStream stream(&result);
        stream << "ID: " << m_id << "\n"
               << "Name: " << m_name << "\n"
               << "Company: " << m_company << "\n"
               << "Salary: " << getSalaryText() << "\n"
               << "Published: " << m_publishedAt;
        if (hasCoords()) {
            stream << "\nCoords: " << m_location.getX() << ", " << m_location.getY();
        }
        return result;
    }

private:
    QString m_id;
    QString m_name;
    QString m_company;
    int m_salaryFrom = 0;
    int m_salaryTo = 0;
    QString m_salaryCurrency;
    QString m_url;
    QString m_publishedAt;
    QString m_description;
    QString m_address;
    QString m_experience;
    QString m_employment;
    QString m_schedule;
    bool m_hasFullData = false;
    Point m_location;  // ← новое поле!
};

// Глобальные операторы
inline QDataStream &operator<<(QDataStream &out, const Vacancy &v) {
    v.serialize(out);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, Vacancy &v) {
    v.deserialize(in);
    return in;
}

inline QDebug operator<<(QDebug debug, const Vacancy &v) {
    QDebugStateSaver saver(debug);
    debug.noquote() << v.toString();
    return debug;
}
