#ifndef DBSELECTOR_H
#define DBSELECTOR_H
#include <QVariantList>
#include "dbtypes.h"
#include "dbmanager.h"

class DBSelector
{
public:
    DBSelector();
    DBTypes::DBResult selectAll(const std::string& tableName,
                                std::vector<QVariantList>& returnData);
    DBTypes::DBResult selectWhere(
        const std::string& table,
        const std::string& where,
        const QVariantList& args,
        std::vector<QVariantList>& out
        );


private:
    DBManager& m_dbManager;
    std::string generateQuery(const std::string& tableName) const;
};

#endif // DBSELECTOR_H
