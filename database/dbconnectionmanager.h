#ifndef DBCONNECTIONMANAGER_H
#define DBCONNECTIONMANAGER_H
#include <memory>
#include <QObject>
#include "dbtypes.h"



class DBConnectionManager
{
public:
    DBConnectionManager();
    ~DBConnectionManager();

    std::string databasePath() const;
    DBTypes::DBState state() const;

    bool isValid() const;
private:
    Q_DISABLE_COPY(DBConnectionManager)

    bool setUp();
    bool setUpWorkspace();
    bool setUpTables();

    void setIsValid(bool isValid);

    class DBManagerPrivate;
    std::unique_ptr<DBManagerPrivate> m_d;
};

#endif // DBMDBCONNECTIONANAGER_H
