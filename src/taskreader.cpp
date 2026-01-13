
#include "include/taskreader.h"
#include "database/dbprocessing.h"
#include "database/dbtypes.h"
#include <iterator>
#include <QDateTime>

using namespace DBTypes;

TaskReader::TaskReader()
{
}

static QDateTime parseDateTime(const QVariant& v)
{
    if (v.isNull())
        return {};

    if (v.metaType().id() == QMetaType::QDateTime)
        return v.toDateTime();

    const QString s = v.toString();
    if (s.isEmpty())
        return {};

    // Try ISO formats first (matches Qt/SQLite default storage)
    QDateTime dt = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (!dt.isValid())
        dt = QDateTime::fromString(s, Qt::ISODate);

    // Fallback to legacy C-locale textual formats
    if (!dt.isValid()) {
        static const QLocale cLocale(QLocale::C);
        dt = cLocale.toDateTime(s, "ddd MMM d HH:mm:ss yyyy");
        if (!dt.isValid())
            dt = cLocale.toDateTime(s, "ddd MMM dd HH:mm:ss yyyy");
    }

    return dt;
}



// DBSelector generates queries as: SELECT rowid, * FROM <table>
// Result columns layout: [rowid, Id, UserId, Title, Description, State, CreatedAt, UpdatedAt, DeletedAt]
// We map to Task(title, description, taskState, createdAt, id)
std::vector<Task> transform(const std::vector<DBEntry>& source)
{
    std::vector<Task> target;
    target.reserve(source.size());

    std::transform(source.begin(), source.end(), std::back_inserter(target),
                   [](const DBEntry& entry)
                   {
                       if (entry.size() < 6)
                           return Task{};

                       const QString title = entry[3].toString();
                       const QString description = entry[4].toString();
                       const auto taskState = static_cast<DBTypes::TaskState>(entry[5].toInt());

                       const QDateTime createdAt  = entry.size() > 6 ? parseDateTime(entry[6]) : QDateTime{};
                       const QDateTime updatedAt  = entry.size() > 7 ? parseDateTime(entry[7]) : QDateTime{};
                       const QDateTime deletedAt  = entry.size() > 8 ? parseDateTime(entry[8]) : QDateTime{};

                       const DBTypes::DBIndex id = entry[1].toInt();

                       Task t{title, description, taskState, createdAt, id};
                       t.setUpdatedAt(updatedAt);
                       t.setDeletedAt(deletedAt);
                       return t;
                   });

    return target;
}


std::pair<bool, std::vector<Task>> TaskReader::requestTaskBrowse(bool showDeleted, int userId)
{
    DBResult result;
    std::vector<DBEntry> entries;
    
    QVariantList args;
    std::string where;

    if (userId >= 0) {
        where = "UserId = ? AND ";
        args << userId;
    }

    if (showDeleted) {
        where += "DeletedAt IS NOT NULL";
    } else {
        where += "DeletedAt IS NULL";
    }

    std::tie(result, entries) = DBProcessing::instance().requestTableDataWhere(DBTables::Tasks, where, args);
    
    return std::make_pair(result == DBResult::OK,
                          transform(entries));
}

TaskReader::~TaskReader()
{
}

