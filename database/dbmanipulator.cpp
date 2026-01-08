#include "dbmanipulator.h"
#include <sstream>

using namespace DBTypes;

namespace {
static const std::map<std::string, std::string> tablesMapping = {
    {"Users", "Login, PasswordHash"},
    {"Tasks", "UserId, Title, Description, State, CreatedAt, UpdatedAt, DeletedAt"},
    {"Callbacks", "UserId, Rate, Description, CreatedAt"}
};
}

DBManipulator::DBManipulator()
    : m_dbManager {DBManager::instance()}
{
}

std::pair<DBResult, int> DBManipulator::insertRow(const std::string& tableName, const QVariantList& rowData)
{
    const std::string& query = generateQuery(tableName, rowData.size());
    const auto& result = m_dbManager.execute(query, rowData);
    return std::make_pair(result.first,
                          result.second.lastInsertId().toInt());
}

DBResult DBManipulator::updateValue(const std::string& tableName, const QVector<QString>& columns, const QVariantList& values)
{
    Q_ASSERT((columns.size() == values.size()) && columns.startsWith("Id"));

    const std::string& query = generateUpdateQuery(tableName, columns, values);
    const auto& result = m_dbManager.execute(query);
    return result.first;
}

DBResult DBManipulator::deleteIdentity(const std::string& tableName, int identity)
{
    std::string query = "DELETE FROM " + tableName + " WHERE rowid = " + std::to_string(identity);
    return m_dbManager.execute(query).first;
}

std::string DBManipulator::generateBindString(size_t paramCount) const
{
    std::ostringstream bindings;
    std::fill_n(std::ostream_iterator<std::string>(bindings),
                paramCount,
                "?,");

    std::string bindString = bindings.str();
    bindString.pop_back();

    return bindString;
}

std::string DBManipulator::generateQuery(const std::string& tableName, size_t paramCount) const
{
    std::string query = "INSERT INTO " + tableName +  " (" + tablesMapping.at(tableName) + ")"
                        " VALUES (";

    query += generateBindString(paramCount);
    query += ")";

    return query;
}

std::string DBManipulator::generateUpdateQuery(const std::string tableName, const QVector<QString>& columns, const QVariantList& values) const
{
    std::string query = "UPDATE " + tableName +
                        " SET " + generateSetString(columns, values) +
                        " WHERE rowid = " + values.first().toString().toStdString();
    return query;
}

std::string DBManipulator::generateSetString(const QVector<QString>& columns, const QVariantList& values) const
{
    std::string result;
    auto columnValue = values.cbegin() + 1; // 0 is rowid
    for (auto column = columns.cbegin() + 1;
         column != columns.cend(); ++column)
    {
        result += column->toStdString() + " = '" + columnValue->toString().toStdString() + "',";
        ++columnValue;
    }
    result.pop_back(); // remove last ","
    return result;
}
