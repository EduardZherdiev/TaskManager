#include "dbselector.h"
#include "dbmanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

using namespace DBTypes;

DBSelector::DBSelector()
    : m_dbManager {DBManager::instance()}
{
}

DBResult DBSelector::selectWhere(
    const std::string& table,
    const std::string& where,
    const QVariantList& args,
    std::vector<QVariantList>& out
    )
{
    std::string queryText = generateQuery(table);

    if (!where.empty()) {
        queryText += " WHERE " + where;
    }

    DBResult result;
    QSqlQuery query;
    std::tie(result, query) = m_dbManager.execute(queryText, args);

    if (result != DBResult::OK)
        return result;

    while (query.next()) {
        const QSqlRecord record = query.record();
        QVariantList row;

        for (int i = 0; i < record.count(); ++i) {
            row.push_back(record.value(i));
        }

        out.push_back(std::move(row));
    }

    return DBResult::OK;
}


DBResult DBSelector::selectAll(const std::string& tableName, std::vector<QVariantList>& returnData)
{
    const std::string query = generateQuery(tableName);
    DBResult result;
    QSqlQuery resultQuery;
    std::tie(result, resultQuery) = m_dbManager.execute(query);

    if (result == DBResult::OK) {
        while (resultQuery.next()) {
            const auto& resultRecord = resultQuery.record();
            QVariantList result;
            for (int i = 0; i < resultRecord.count(); ++i) {
                result.push_back(resultRecord.value(i));
            }
            returnData.push_back(std::move(result));
        }
    }

    return result;
}



std::string DBSelector::generateQuery(const std::string& tableName) const
{
    std::string query = "SELECT rowid, * FROM " + tableName;
    return query;
}
