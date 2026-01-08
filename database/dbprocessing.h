#ifndef DBPROCESSING_H
#define DBPROCESSING_H
#include <vector>
#include <QVariantList>
#include "dbtypes.h"
#include "dbselector.h"
#include "dbmanipulator.h"
//#define INSERT_TEST_DATA

class DBProcessing
{
public:
    DBProcessing();
    ~DBProcessing();
#ifdef INSERT_TEST_DATA
    void insertTestData();
#endif

    std::pair<DBTypes::DBResult,
              std::vector<QVariantList>> requestTableData(DBTypes::DBTables table);

    std::pair<DBTypes::DBResult,
              std::vector<QVariantList>> requestTableDataWhere(DBTypes::DBTables table, const std::string& where, const QVariantList& args);

    std::pair<DBTypes::DBResult,
              int> requestAddRow(DBTypes::DBTables table, const QVariantList& data);

    DBTypes::DBResult requestUpdate(DBTypes::DBTables table, const QVector<QString>& headers, const QVariantList& data);
    DBTypes::DBResult requestDelete(DBTypes::DBTables table, int identity);
private:
    DBManipulator m_manipulator;
    DBSelector m_selector;
};

#endif // DBPROCESSING_H


