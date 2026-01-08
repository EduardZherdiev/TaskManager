#include "userreader.h"
#include "database/dbprocessing.h"
#include "database/dbtypes.h"
#include <iterator>

using namespace DBTypes;

UserReader::UserReader()
    : m_dbProcessor{new DBProcessing{}}
{
}

// DBSelector: SELECT rowid, * FROM <table>
// Expected Users columns: [rowid, Id, Login, PasswordHash]
static std::vector<User> transformUsers(const std::vector<DBEntry>& source)
{
    std::vector<User> target;
    std::transform(source.begin(), source.end(), std::back_inserter(target),
                   [](const DBEntry& entry) {
                       if (entry.size() < 3)
                           return User{};

                       const QString login = entry[2].toString();
                       const QString passwordHash = entry.size() > 3 ? entry[3].toString() : QString{};
                       const DBIndex id = entry.size() > 1 ? entry[1].toInt() : 0;

                       return User{login, passwordHash, id};
                   });
    return target;
}

std::pair<bool, std::vector<User>> UserReader::requestUserBrowse()
{
    DBResult result;
    std::vector<DBEntry> entries;
    std::tie(result, entries) = m_dbProcessor->requestTableData(DBTables::Users);
    return std::make_pair(result == DBResult::OK, transformUsers(entries));
}

UserReader::~UserReader()
{
}
