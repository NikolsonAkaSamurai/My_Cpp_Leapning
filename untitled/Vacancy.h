// vacancy.h
#ifndef VACANCY_H
#define VACANCY_H

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

class Vacancy {
public:
    static const quint32 SERIALIZATION_VERSION = 1;

    // Конструкторы
    Vacancy();
    Vacancy(const QString &id, const QString &name, const QString &company);
    Vacancy(const Vacancy &other);
    ~Vacancy() = default;

    // Операторы
    Vacancy& operator=(const Vacancy &other);
    bool operator==(const Vacancy &other) const;
    bool operator!=(const Vacancy &other) const;

    // Геттеры
    QString getId() const;
    QString getName() const;
    QString getCompany() const;
    int getSalaryFrom() const;
    int getSalaryTo() const;
    QString getSalaryCurrency() const;
    QString getUrl() const;
    QString getPublishedAt() const;
    QDateTime getPublishedDateTime() const;
    QString getDescription() const;
    QString getAddress() const;
    QString getExperience() const;
    QString getEmployment() const;
    QString getSchedule() const;
    bool hasFullData() const;
    bool hasSalary() const;
    int getAverageSalary() const;
    QString getSalaryText() const;

    // Сеттеры
    void setId(const QString &id);
    void setName(const QString &name);
    void setCompany(const QString &company);
    void setSalaryFrom(int salary);
    void setSalaryTo(int salary);
    void setSalaryCurrency(const QString &currency);
    void setUrl(const QString &url);
    void setPublishedAt(const QString &publishedAt);
    void setDescription(const QString &description);
    void setAddress(const QString &address);
    void setExperience(const QString &experience);
    void setEmployment(const QString &employment);
    void setSchedule(const QString &schedule);
    void setFullDataFlag(bool full);

    // Метод для очистки полных данных - ЭТОТ МЕТОД НУЖНО ДОБАВИТЬ!
    void clearFullData();

    // JSON методы
    QJsonObject toJson() const;
    static Vacancy fromJson(const QJsonObject &obj);

    // Сериализация
    void serialize(QDataStream &out) const;
    void deserialize(QDataStream &in);

    // Для отладки
    void print() const;
    QString toString() const;

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
};

QDataStream &operator<<(QDataStream &out, const Vacancy &v);
QDataStream &operator>>(QDataStream &in, Vacancy &v);
QDebug operator<<(QDebug debug, const Vacancy &v);

#endif // VACANCY_H