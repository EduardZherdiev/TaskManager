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

static const std::map<DBTypes::DBTables, std::string> tableMapper {
    {DBTypes::DBTables::Users, "Users"},
    {DBTypes::DBTables::Tasks, "Tasks"},
    {DBTypes::DBTables::Callbacks, "Callbacks"}
};

static const std::map<DBTypes::DBTables, std::string> selectionMapper {
    {DBTypes::DBTables::Users, "Users"},
    {DBTypes::DBTables::Tasks, "Tasks"},
    {DBTypes::DBTables::Callbacks, "Callbacks"}
};

#ifdef INSERT_TEST_DATA
void DBProcessing::insertTestData()
{
    DBManipulator dbManager;

    // Create users (insert only if not exists)
    int aliceId = 0;
    int bobId = 0;
    int carolId = 0;

    {
        std::vector<QVariantList> out;
        if (m_selector.selectWhere("Users", "Login = ?", QVariantList{"alice"}, out) == DBTypes::DBResult::OK && !out.empty())
            aliceId = out.front().size() > 1 ? out.front()[1].toInt() : 0;
        else
            aliceId = dbManager.insertRow("Users", {{"alice"}, {"hash_alice"}}).second;
    }

    {
        std::vector<QVariantList> out;
        if (m_selector.selectWhere("Users", "Login = ?", QVariantList{"bob"}, out) == DBTypes::DBResult::OK && !out.empty())
            bobId = out.front().size() > 1 ? out.front()[1].toInt() : 0;
        else
            bobId = dbManager.insertRow("Users", {{"bob"}, {"hash_bob"}}).second;
    }

    {
        std::vector<QVariantList> out;
        if (m_selector.selectWhere("Users", "Login = ?", QVariantList{"carol"}, out) == DBTypes::DBResult::OK && !out.empty())
            carolId = out.front().size() > 1 ? out.front()[1].toInt() : 0;
        else
            carolId = dbManager.insertRow("Users", {{"carol"}, {"hash_carol"}}).second;
    }

    // Create tasks for users (insert only if not exists by title+user)
    auto insertTaskIfMissing = [&](int userId, const QString& title, const QString& desc, int state, const QDateTime& createdAt, const QDateTime& updatedAt) {
        std::vector<QVariantList> out;
        QString where = "Title = ? AND UserId = ?";
        QVariantList args{title, userId};
        if (m_selector.selectWhere("Tasks", where.toStdString(), args, out) == DBTypes::DBResult::OK && !out.empty())

            return out.front().size() > 1 ? out.front()[1].toInt() : 0;
        return dbManager.insertRow("Tasks", {QVariant(userId), QVariant(title), QVariant(desc), QVariant(state), QVariant(createdAt.toString(Qt::ISODate)), QVariant(updatedAt.isValid() ? updatedAt.toString(Qt::ISODate) : QVariant()), QVariant()}).second;
    };

    insertTaskIfMissing(aliceId, "Buy groceries", "Milk, Bread, Eggs", 0, QDateTime::currentDateTime(), QDateTime());
    insertTaskIfMissing(bobId, "Finish report", "Monthly sales report", 0, QDateTime::currentDateTime().addDays(-2), QDateTime::currentDateTime());
    insertTaskIfMissing(carolId, "Plan trip", "Book flights and hotels", 1, QDateTime::currentDateTime().addDays(-10), QDateTime::currentDateTime().addDays(-1));

    // Create callbacks (insert only if not exists by user+description)
    auto insertCallbackIfMissing = [&](int userId, int rate, const QString& desc, const QDateTime& createdAt) {
        std::vector<QVariantList> out;
        QString where = "UserId = ? AND Description = ?";
        QVariantList args{userId, desc};
        if (m_selector.selectWhere("Callbacks", where.toStdString(), args, out) == DBTypes::DBResult::OK && !out.empty())
            return out.front().size() > 1 ? out.front()[1].toInt() : 0;
        return dbManager.insertRow("Callbacks", {QVariant(userId), QVariant(rate), QVariant(desc), QVariant(createdAt.toString(Qt::ISODate))}).second;
    };

    insertCallbackIfMissing(aliceId, 5, "Great service", QDateTime::currentDateTime());
    insertCallbackIfMissing(bobId, 4, "Good support", QDateTime::currentDateTime().addDays(-3));
    insertCallbackIfMissing(carolId, 3, "Average experience", QDateTime::currentDateTime().addDays(-7));
}
#endif

std::pair<DBTypes::DBResult, std::vector<QVariantList>> DBProcessing::requestTableData(DBTypes::DBTables table)
{
    std::vector<QVariantList> result;
    DBTypes::DBResult resultState = m_selector.selectAll(selectionMapper.at(table), result);
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
