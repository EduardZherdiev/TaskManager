#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMap>
#include <QDebug>
#include "dbconnectionmanager.h"

class DBManager
{
public:
    Q_DISABLE_COPY(DBManager)

    static DBManager& instance();

    template<class ...Args>
    std::pair<DBTypes::DBResult, QSqlQuery> execute(const std::string& queryText, Args... args)
    {
        QSqlQuery query;
        query.prepare(QString::fromStdString(queryText));

        bindValue(query, 0, args...);

        Q_ASSERT(query.boundValues().size() == sizeof...(args));

        DBTypes::DBResult result = DBTypes::DBResult::OK;
        if(!query.exec() && query.lastError().isValid()) {
            qWarning() << query.lastError().text()<< query.lastQuery();
            result = DBTypes::DBResult::FAIL;
        }

        return std::make_pair(result, query);
    }

    std::pair<DBTypes::DBResult, QSqlQuery> execute(const std::string& queryText, const QVariantList &args);

    void bindValue(QSqlQuery&, int) {} // dummy

    template<class T, class ...Args>
    void bindValue(QSqlQuery& query, int index, T data, Args... args)
    {
        query.bindValue(index, data);
        bindValue(query, ++index, args...);
    }
private:
    DBManager() = default;
    DBConnectionManager m_connectionManager;
};

#endif // DBMANAGER_H
