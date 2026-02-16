// vacancy.cpp
#include "vacancy.h"

// ==================== КОНСТРУКТОРЫ ====================

Vacancy::Vacancy() {}

Vacancy::Vacancy(const QString &id, const QString &name, const QString &company)
    : m_id(id)
    , m_name(name)
    , m_company(company)
{
    m_url = QString("https://hh.ru/vacancy/%1").arg(id);
}

Vacancy::Vacancy(const Vacancy &other)
    : m_id(other.m_id)
    , m_name(other.m_name)
    , m_company(other.m_company)
    , m_salaryFrom(other.m_salaryFrom)
    , m_salaryTo(other.m_salaryTo)
    , m_salaryCurrency(other.m_salaryCurrency)
    , m_url(other.m_url)
    , m_publishedAt(other.m_publishedAt)
    , m_description(other.m_description)
    , m_address(other.m_address)
    , m_experience(other.m_experience)
    , m_employment(other.m_employment)
    , m_schedule(other.m_schedule)
    , m_hasFullData(other.m_hasFullData)
{}

// ==================== ОПЕРАТОРЫ ====================

Vacancy& Vacancy::operator=(const Vacancy &other) {
    if (this != &other) {
        m_id = other.m_id;
        m_name = other.m_name;
        m_company = other.m_company;
        m_salaryFrom = other.m_salaryFrom;
        m_salaryTo = other.m_salaryTo;
        m_salaryCurrency = other.m_salaryCurrency;
        m_url = other.m_url;
        m_publishedAt = other.m_publishedAt;
        m_description = other.m_description;
        m_address = other.m_address;
        m_experience = other.m_experience;
        m_employment = other.m_employment;
        m_schedule = other.m_schedule;
        m_hasFullData = other.m_hasFullData;
    }
    return *this;
}

bool Vacancy::operator==(const Vacancy &other) const {
    return m_id == other.m_id;
}

bool Vacancy::operator!=(const Vacancy &other) const {
    return !(*this == other);
}

// ==================== ГЕТТЕРЫ ====================

QString Vacancy::getId() const { return m_id; }
QString Vacancy::getName() const { return m_name; }
QString Vacancy::getCompany() const { return m_company; }
int Vacancy::getSalaryFrom() const { return m_salaryFrom; }
int Vacancy::getSalaryTo() const { return m_salaryTo; }
QString Vacancy::getSalaryCurrency() const { return m_salaryCurrency; }
QString Vacancy::getUrl() const { return m_url; }
QString Vacancy::getPublishedAt() const { return m_publishedAt; }
QString Vacancy::getDescription() const { return m_description; }
QString Vacancy::getAddress() const { return m_address; }
QString Vacancy::getExperience() const { return m_experience; }
QString Vacancy::getEmployment() const { return m_employment; }
QString Vacancy::getSchedule() const { return m_schedule; }
bool Vacancy::hasFullData() const { return m_hasFullData; }

QDateTime Vacancy::getPublishedDateTime() const {
    return QDateTime::fromString(m_publishedAt, Qt::ISODate);
}

bool Vacancy::hasSalary() const {
    return (m_salaryFrom > 0 || m_salaryTo > 0) &&
           (m_salaryCurrency == "RUR" || m_salaryCurrency == "RUB");
}

int Vacancy::getAverageSalary() const {
    if (!hasSalary()) return 0;
    if (m_salaryFrom > 0 && m_salaryTo > 0) return (m_salaryFrom + m_salaryTo) / 2;
    if (m_salaryFrom > 0) return m_salaryFrom;
    return m_salaryTo;
}

QString Vacancy::getSalaryText() const {
    if (!hasSalary()) return "з/п не указана";
    QString currency = (m_salaryCurrency == "RUR" || m_salaryCurrency == "RUB") ? "₽" : m_salaryCurrency;
    if (m_salaryFrom > 0 && m_salaryTo > 0) return QString("%1 - %2 %3").arg(m_salaryFrom).arg(m_salaryTo).arg(currency);
    if (m_salaryFrom > 0) return QString("от %1 %3").arg(m_salaryFrom).arg(currency);
    return QString("до %1 %3").arg(m_salaryTo).arg(currency);
}

// ==================== СЕТТЕРЫ ====================

void Vacancy::setId(const QString &id) { m_id = id; }
void Vacancy::setName(const QString &name) { m_name = name; }
void Vacancy::setCompany(const QString &company) { m_company = company; }
void Vacancy::setSalaryFrom(int salary) { m_salaryFrom = salary; }
void Vacancy::setSalaryTo(int salary) { m_salaryTo = salary; }
void Vacancy::setSalaryCurrency(const QString &currency) { m_salaryCurrency = currency; }
void Vacancy::setUrl(const QString &url) { m_url = url; }
void Vacancy::setPublishedAt(const QString &publishedAt) { m_publishedAt = publishedAt; }
void Vacancy::setDescription(const QString &description) { m_description = description; }
void Vacancy::setAddress(const QString &address) { m_address = address; }
void Vacancy::setExperience(const QString &experience) { m_experience = experience; }
void Vacancy::setEmployment(const QString &employment) { m_employment = employment; }
void Vacancy::setSchedule(const QString &schedule) { m_schedule = schedule; }
void Vacancy::setFullDataFlag(bool full) { m_hasFullData = full; }

void Vacancy::clearFullData() {
    m_description.clear();
    m_address.clear();
    m_experience.clear();
    m_employment.clear();
    m_schedule.clear();
    m_hasFullData = false;
}

// ==================== JSON МЕТОДЫ ====================

QJsonObject Vacancy::toJson() const {
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
    return obj;
}

Vacancy Vacancy::fromJson(const QJsonObject &obj) {
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
    return v;
}

// ==================== СЕРИАЛИЗАЦИЯ ====================

void Vacancy::serialize(QDataStream &out) const {
    out << SERIALIZATION_VERSION
        << m_id << m_name << m_company
        << m_salaryFrom << m_salaryTo << m_salaryCurrency
        << m_url << m_publishedAt
        << m_description << m_address << m_experience
        << m_employment << m_schedule
        << m_hasFullData;
}

void Vacancy::deserialize(QDataStream &in) {
    quint32 version;
    in >> version;
    if (version == 1) {
        in >> m_id >> m_name >> m_company
           >> m_salaryFrom >> m_salaryTo >> m_salaryCurrency
           >> m_url >> m_publishedAt
           >> m_description >> m_address >> m_experience
           >> m_employment >> m_schedule
           >> m_hasFullData;
    }
}

// ==================== ОТЛАДКА ====================

void Vacancy::print() const {
    qDebug().noquote() << toString();
}

QString Vacancy::toString() const {
    QString result;
    QTextStream stream(&result);
    stream << "ID: " << m_id << "\n"
           << "Name: " << m_name << "\n"
           << "Company: " << m_company << "\n"
           << "Salary: " << getSalaryText() << "\n"
           << "Published: " << m_publishedAt;
    return result;
}

// ==================== ГЛОБАЛЬНЫЕ ОПЕРАТОРЫ ====================

QDataStream &operator<<(QDataStream &out, const Vacancy &v) {
    v.serialize(out);
    return out;
}

QDataStream &operator>>(QDataStream &in, Vacancy &v) {
    v.deserialize(in);
    return in;
}

QDebug operator<<(QDebug debug, const Vacancy &v) {
    QDebugStateSaver saver(debug);
    debug.noquote() << v.toString();
    return debug;
}