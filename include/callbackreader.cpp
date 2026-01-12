#include "callbackreader.h"
#include "database/dbprocessing.h"
#include "database/dbtypes.h"
#include <iterator>
#include <QDateTime>

using namespace DBTypes;

CallbackReader::CallbackReader()
{
}

// DBSelector uses: SELECT rowid, * FROM <table>
// Expected Callbacks columns: [rowid, Id, Rate, Description, CreatedAt]
static std::vector<Callback> transformCallbacks(const std::vector<DBEntry>& source)
{
    std::vector<Callback> target;
    std::transform(source.begin(), source.end(), std::back_inserter(target),
                   [](const DBEntry& entry) {
                       if (entry.size() < 4)
                           return Callback{};

                       const int rate = entry[2].toInt();
                       const QString description = entry[3].toString();
                       const QDateTime createdAt = entry.size() > 4 ? entry[4].toDateTime() : QDateTime{};
                       const DBIndex id = entry.size() > 1 ? entry[1].toInt() : 0;

                       return Callback{rate, description, createdAt, id};
                   });
    return target;
}

std::pair<bool, std::vector<Callback>> CallbackReader::requestCallbackBrowse()
{
    DBResult result;
    std::vector<DBEntry> entries;
    std::tie(result, entries) = DBProcessing::instance().requestTableData(DBTables::Callbacks);
    return std::make_pair(result == DBResult::OK, transformCallbacks(entries));
}

CallbackReader::~CallbackReader()
{
}
