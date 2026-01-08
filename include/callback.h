#pragma once
#include <QString>
#include <QDateTime>
#include "database/dbtypes.h"

class Callback {
public:
    Callback() = default;
    Callback(int rate, QString description, QDateTime createdAt, DBTypes::DBIndex id);

    DBTypes::DBIndex id() const;
    int rate() const;
    QString description() const;
    QDateTime createdAt() const;

private:
    DBTypes::DBIndex m_id{0};
    int m_rate{0};
    QString m_description;
    QDateTime m_createdAt;
};
