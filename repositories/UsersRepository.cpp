// UsersRepository.cpp
#include "UsersRepository.h"
#include "../QueryExecutor.h"

using namespace DBTypes;

DBResult db::UsersRepository::addUser(
    const QString& name,
    const QString& email)
{
    return QueryExecutor::exec(
        "INSERT INTO Users(name, email) VALUES (?, ?)",
        { name, email }
        );
}

DBResult db::UsersRepository::removeUser(DBId id)
{
    return QueryExecutor::exec(
        "DELETE FROM Users WHERE id = ?",
        { id }
        );
}

QList<DBEntry> db::UsersRepository::getAllUsers()
{
    return QueryExecutor::select(
        "SELECT id, name, email FROM Users"
        );
}
