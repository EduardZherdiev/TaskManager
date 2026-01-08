#ifndef DBINSERTER_H
#define DBINSERTER_H
#include "dbmanager.h"
#include "dbtypes.h"

class DBManipulator
{
public:
    DBManipulator();

    std::pair<DBTypes::DBResult, int> insertRow(const std::string& tableName, const QVariantList& rowData);

    template<class ...Args>
    DBTypes::DBResult insertRow(const std::string& tableName, Args... args)
    {
        const std::string& query = generateQuery(tableName, sizeof...(args));
        return m_dbManager.execute(query, std::forward(args)...).first;
    }

    DBTypes::DBResult updateValue(const std::string& tableName, const QVector<QString>& columns, const QVariantList& values);
    DBTypes::DBResult deleteIdentity(const std::string& tableName, int identity);
private:
    DBManager& m_dbManager;
    std::string generateBindString(size_t paramCount) const;
    std::string generateQuery(const std::string& tableName, size_t paramCount) const;
    std::string generateUpdateQuery(const std::string tableName,
                                    const QVector<QString>& columns, const QVariantList& values) const;
    std::string generateSetString(const QVector<QString>& columns, const QVariantList& values) const;
};

#endif // DBINSERTER_H
