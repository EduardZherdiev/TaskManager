#include "include/user.h"

User::User(QString login, QString passwordHash, DBTypes::DBIndex id)
    : m_id{id}, m_login{std::move(login)}, m_passwordHash{std::move(passwordHash)}
{
}

DBTypes::DBIndex User::id() const { return m_id; }
QString User::login() const { return m_login; }
QString User::passwordHash() const { return m_passwordHash; }
