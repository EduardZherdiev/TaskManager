#pragma once
#include <QString>
#include "database/dbtypes.h"

class User {
public:
    User() = default;
    User(QString login, QString passwordHash, DBTypes::DBIndex id);

    DBTypes::DBIndex id() const;
    QString login() const;
    QString passwordHash() const;

private:
    DBTypes::DBIndex m_id{0};
    QString m_login;
    QString m_passwordHash;
};
