#include "dbconnectionmanager.h"
#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

using namespace DBTypes;

namespace
{
    class DBCloser {
    public:
        void operator() (QSqlDatabase* db) {
            db->close();
        }
    };
}

class DBConnectionManager::DBManagerPrivate {
public:
    mutable bool m_isValid {true};
    std::unique_ptr<QSqlDatabase, DBCloser> m_database;
    std::string m_dbPath;
    DBState m_state {DBState::OK};
};

std::string DBConnectionManager::databasePath() const
{
    return m_d->m_dbPath;
}

DBState DBConnectionManager::state() const
{
    return m_d->m_state;
}

bool DBConnectionManager::setUp()
{
    const QString driver {"QSQLITE"};

    if (!QSqlDatabase::isDriverAvailable(driver)) {
        m_d->m_state = DBState::ERROR_NO_DRIVER;
        qWarning() << "Driver " << driver << " is not available.";
        return false;
    }

    if (!setUpWorkspace()) {
        m_d->m_state = DBState::ERROR_WORKSPACE;
        qWarning() << "Workspace setup failed!";
        return false;
    }

    auto* db = new QSqlDatabase {QSqlDatabase::addDatabase(driver)};
    m_d->m_database.reset(db);
    m_d->m_database->setDatabaseName(QString::fromStdString(m_d->m_dbPath));

    qDebug() << "Database name: " << m_d->m_database->databaseName();

    if (!m_d->m_database->open()) {
        m_d->m_state = DBState::ERROR_OPENING;
        qWarning() << "Error in opening DB " << m_d->m_database->databaseName()
                   << " reason: " <<  m_d->m_database->lastError().text();
        return false;
    }

    return setUpTables();
}

bool DBConnectionManager::setUpWorkspace()
{
#ifdef BUILD_TESTS
    const QString databaseName {"TestDB"};
#else
    const QString databaseName {"TaskManagerDB"};
#endif

    // Place DB file next to the application (project) folder.
    const QString location {QCoreApplication::applicationDirPath()};
    const QString fullPath {location + "/" + databaseName + ".sqlite"};

    m_d->m_dbPath = fullPath.toStdString();

    QDir dbDirectory {location};
    if (!dbDirectory.exists()) {
        qWarning() << "DB directory does not exist, creating result: "
                   << dbDirectory.mkpath(location);
    }

    qDebug() << "Database path: " << fullPath;

    return true;
}

bool DBConnectionManager::setUpTables()
{
    bool result = true;

    QSqlQuery pragma;
    pragma.exec("PRAGMA foreign_keys = ON;");

    const std::vector<QString> queries = {
        R"(
        CREATE TABLE IF NOT EXISTS Users (
            Id INTEGER PRIMARY KEY,
            Login TEXT UNIQUE NOT NULL,
            PasswordHash TEXT NOT NULL,
            CHECK (length(trim(Login)) > 0),
            CHECK (length(trim(PasswordHash)) > 0)
        );
        )",

        R"(
        CREATE TABLE IF NOT EXISTS Tasks (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            UserId INTEGER NOT NULL,

            Title TEXT NOT NULL,
            Description TEXT,
            State INTEGER NOT NULL,

            CreatedAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            UpdatedAt DATETIME,
            DeletedAt DATETIME,

            FOREIGN KEY (UserId) REFERENCES Users(Id) ON DELETE CASCADE,
            CHECK (length(trim(Title)) > 0),
            CHECK (State BETWEEN 0 AND 2)
        );
        )",

        R"(
        CREATE TABLE IF NOT EXISTS Feedbacks (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            UserId INTEGER NOT NULL,
            Rate INTEGER NOT NULL,
            Description TEXT,
            CreatedAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,

            FOREIGN KEY (UserId) REFERENCES Users(Id) ON DELETE CASCADE,
            CHECK (Rate BETWEEN 1 AND 5)
        );
        )"
    };

    QSqlQuery query;

    for (const auto& sql : queries) {
        if (!query.exec(sql)) {
            result = false;
            m_d->m_state = DBState::ERROR_TABLES;
            qWarning() << "Table creation failed:" << query.lastError().text();
        } else {
            qDebug() << "Table created successfully";
        }
    }

    const std::vector<QString> indexQueries = {
        "CREATE INDEX IF NOT EXISTS idx_tasks_user_id ON Tasks(UserId);",
        "CREATE INDEX IF NOT EXISTS idx_tasks_state ON Tasks(State);",
        "CREATE INDEX IF NOT EXISTS idx_tasks_created_at ON Tasks(CreatedAt);",
        "CREATE INDEX IF NOT EXISTS idx_Feedbacks_user_id ON Feedbacks(UserId);"
    };

    for (const auto& sql : indexQueries)
        if (!query.exec(sql)) {
            result = false;
            m_d->m_state = DBState::ERROR_TABLES;
            qWarning() << "Inex creation failed:" << query.lastError().text();
        } else {
            qDebug() << "Inex created successfully";
        }

    return result;
}


void DBConnectionManager::setIsValid(bool isValid)
{
    m_d->m_isValid = isValid;
}

bool DBConnectionManager::isValid() const
{
    return m_d->m_isValid && m_d->m_database->isValid();
}

DBConnectionManager::DBConnectionManager()
    : m_d {new DBManagerPrivate {}}
{
    setIsValid(setUp());
}

DBConnectionManager::~DBConnectionManager()
{
}

