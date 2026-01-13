#include "dbprocessing.h"
#include "dbmanipulator.h"
#include <QDateTime>
#include <QCryptographicHash>

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

    // Clear existing test data
    dbManager.deleteAll("Tasks");
    dbManager.deleteAll("Feedbacks");
    dbManager.deleteAll("Users");

    // Helper function to hash passwords
    auto hashPassword = [](const QString& password) -> QString {
        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        return QString(hash.toHex());
    };

    // Create users with hashed passwords
    // Passwords: alice = "alice", bob = "bob", carol = "carol"
    int aliceId = 0;
    int bobId = 0;
    int carolId = 0;

    aliceId = dbManager.insertRow("Users", {{"alice"}, {hashPassword("alice")}}).second;
    bobId = dbManager.insertRow("Users", {{"bob"}, {hashPassword("bob")}}).second;
    carolId = dbManager.insertRow("Users", {{"carol"}, {hashPassword("carol")}}).second;

    // Create tasks for users
    auto insertTask = [&](int userId, const QString& title, const QString& desc, int state, const QDateTime& createdAt, const QDateTime& updatedAt) {
        return dbManager.insertRow("Tasks", {QVariant(userId), QVariant(title), QVariant(desc), QVariant(state), QVariant(createdAt.toString(Qt::ISODate)), QVariant(updatedAt.isValid() ? updatedAt.toString(Qt::ISODate) : QString()), QVariant()}).second;
    };

    insertTask(aliceId, "Buy groceries", "Milk, Bread, Eggs", 0, QDateTime::currentDateTime(), QDateTime());
    insertTask(bobId, "Finish report", "Monthly sales report", 0, QDateTime::currentDateTime().addDays(-2), QDateTime::currentDateTime());
    insertTask(carolId, "Plan trip", "Book flights and hotels", 1, QDateTime::currentDateTime().addDays(-10), QDateTime::currentDateTime().addDays(-1));

    // Add 200 tasks for bob
    for (int i = 1; i <= 200; ++i) {
        QString title = QString("Task %1 for Bob").arg(i);
        QString description = QString("Description for task %1. This is a sample task with some content.").arg(i);
        int state = i % 3;  // 0=Active, 1=Completed, 2=Archived
        QDateTime created = QDateTime::currentDateTime().addDays(-(200 - i));
        QDateTime updated = (state == 1) ? QDateTime::currentDateTime().addDays(-(200 - i) + 1) : QDateTime();
        insertTask(bobId, title, description, state, created, updated);
    }

    // Create Feedbacks
    auto insertFeedback = [&](int userId, int rate, const QString& desc, const QDateTime& createdAt) {
        return dbManager.insertRow("Feedbacks", {QVariant(userId), QVariant(rate), QVariant(desc), QVariant(createdAt.toString(Qt::ISODate))}).second;
    };

    insertFeedback(aliceId, 5, "Great service", QDateTime::currentDateTime());
    insertFeedback(bobId, 4, "Good support", QDateTime::currentDateTime().addDays(-3));
    insertFeedback(carolId, 3, "Average experience", QDateTime::currentDateTime().addDays(-7));

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
