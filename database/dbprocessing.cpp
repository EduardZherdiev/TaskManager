#include "dbprocessing.h"
#include "dbmanipulator.h"
#include <QDateTime>

DBProcessing::DBProcessing()
{
#ifdef INSERT_TEST_DATA
    insertTestData();
#endif
}

DBProcessing::~DBProcessing()
{
}

DBProcessing& DBProcessing::instance()
{
    static DBProcessing s_instance;
    return s_instance;
}

static const std::map<DBTypes::DBTables, std::string> tableMapper {
    {DBTypes::DBTables::Users, "Users"},
    {DBTypes::DBTables::Tasks, "Tasks"},
    {DBTypes::DBTables::Feedbacks, "Feedbacks"}
};

static const std::map<DBTypes::DBTables, std::string> selectionMapper {
    {DBTypes::DBTables::Users, "Users"},
    {DBTypes::DBTables::Tasks, "Tasks"},
    {DBTypes::DBTables::Feedbacks, "Feedbacks"}
};

#ifdef INSERT_TEST_DATA
void DBProcessing::insertTestData()
{
    DBManipulator dbManager;
    qDebug()<<"Begin insert test data";
    // Use existing Bob user ID (Bob is registered separately)
    int bobId = 0;
    QVariantList args;
    args << "Bob";
    std::vector<QVariantList> bobRows;
    DBTypes::DBResult bobResult = m_selector.selectWhere("Users", "Login = ?", args, bobRows);
    if (bobResult == DBTypes::DBResult::OK && !bobRows.empty()) {
        const auto &row = bobRows.front();
        if (row.size() >= 2) {
            bobId = row[1].toInt();
        }
    }
    qDebug() << "bob id = "<< bobId;
    if (bobId <= 0) {
        qWarning() << "Test data skipped: Bob user not found";
        return;
    }

    // Create tasks for users
    auto insertTask = [&](int userId, const QString& title, const QString& desc, int state, const QDateTime& createdAt, const QDateTime& updatedAt) {
        return dbManager.insertRow("Tasks", {QVariant(), QVariant(userId), QVariant(title), QVariant(desc), QVariant(state), QVariant(createdAt.toString(Qt::ISODate)), QVariant(updatedAt.isValid() ? updatedAt.toString(Qt::ISODate) : QString()), QVariant()}).second;
    };

    const QDateTime nowUtc = QDateTime::currentDateTimeUtc();
    insertTask(bobId, "Finish report", "Monthly sales report", 0, nowUtc.addDays(-2), nowUtc);

    // Add 200 tasks for bob
    for (int i = 1; i <= 200; ++i) {
        QString title = QString("Task %1 for Bob").arg(i);
        QString description = QString("Description for task %1. This is a sample task with some content.").arg(i);
        int state = i % 3;  // 0=Active, 1=Completed, 2=Archived
        QDateTime created = nowUtc.addDays(-(200 - i));
        QDateTime updated = (state == 1) ? nowUtc.addDays(-(200 - i) + 1) : QDateTime();
        insertTask(bobId, title, description, state, created, updated);
    }

    // Create Feedbacks
    auto insertFeedback = [&](int userId, int rate, const QString& desc, const QDateTime& createdAt) {
        return dbManager.insertRow("Feedbacks", {QVariant(userId), QVariant(rate), QVariant(desc), QVariant(createdAt.toString(Qt::ISODate))}).second;
    };

    insertFeedback(bobId, 4, "Good support", nowUtc.addDays(-3));

    qDebug() << "Test data inserted successfully";
}
#endif

std::pair<DBTypes::DBResult, std::vector<QVariantList>> DBProcessing::requestTableData(DBTypes::DBTables table)
{
    std::vector<QVariantList> result;
    DBTypes::DBResult resultState = m_selector.selectAll(selectionMapper.at(table), result);
    return std::make_pair(resultState, std::move(result));
}

std::pair<DBTypes::DBResult, std::vector<QVariantList>> DBProcessing::requestTableDataWhere(DBTypes::DBTables table, const std::string& where, const QVariantList& args)
{
    std::vector<QVariantList> result;
    DBTypes::DBResult resultState = m_selector.selectWhere(selectionMapper.at(table), where, args, result);
    return std::make_pair(resultState, std::move(result));
}

std::pair<DBTypes::DBResult, int> DBProcessing::requestAddRow(DBTypes::DBTables table, const QVariantList& data)
{
    return m_manipulator.insertRow(tableMapper.at(table), data);
}

DBTypes::DBResult DBProcessing::requestUpdate(DBTypes::DBTables table, const QVector<QString>& headers, const QVariantList& data)
{
    return m_manipulator.updateValue(tableMapper.at(table), headers, data);
}

DBTypes::DBResult DBProcessing::requestDelete(DBTypes::DBTables table, int identity)
{
    return m_manipulator.deleteIdentity(tableMapper.at(table), identity);
}
