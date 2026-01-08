// UsersRepository.h
#pragma once
#include "../dbtypes.h"

namespace db {

class UsersRepository {
public:
    static DBTypes::DBResult addUser(
        const QString& name,
        const QString& email
        );

    static DBTypes::DBResult removeUser(DBTypes::DBId id);

    static QList<DBTypes::DBEntry> getAllUsers();
};

}
