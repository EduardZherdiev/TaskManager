#include "dbmanager.h"

using namespace DBTypes;

DBManager&DBManager::instance()
{
    static DBManager instance;
    return instance;
}

std::pair<DBResult, QSqlQuery> DBManager::execute(const std::string& queryText, const QVariantList& args)
{
    QSqlQuery query;
    query.prepare(QString::fromStdString(queryText));

    for(int i = 0; i < args.size(); ++i) {
        query.bindValue(i, args[i]);
    }

    Q_ASSERT(query.boundValues().size() == args.size());

    DBResult result = DBResult::OK;
    if(!query.exec() && query.lastError().isValid()) {
        qWarning() << query.lastError().text() << query.lastQuery() << "bound:" << query.boundValues();
        result = DBResult::FAIL;
    }

    return std::make_pair(result, query);
}
