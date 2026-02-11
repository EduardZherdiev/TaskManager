#include "include/feedbackreader.h"
#include "database/dbprocessing.h"
#include "database/dbtypes.h"
#include <iterator>
#include <QDateTime>

using namespace DBTypes;

FeedbackReader::FeedbackReader()
{
}

// DBSelector uses: SELECT rowid, * FROM <table>
// Expected Feedbacks columns: [rowid, Id, Rate, Description, CreatedAt]
static std::vector<Feedback> transformFeedbacks(const std::vector<DBEntry>& source)
{
    std::vector<Feedback> target;
    target.reserve(source.size());
    std::transform(source.begin(), source.end(), std::back_inserter(target),
                   [](const DBEntry& entry) {
                       if (entry.size() < 4)
                           return Feedback{};

                       const int rate = entry[2].toInt();
                       const QString description = entry[3].toString();
                       const QDateTime createdAt = entry.size() > 4 ? entry[4].toDateTime() : QDateTime{};
                       const DBIndex id = entry.size() > 1 ? entry[1].toInt() : 0;

                       return Feedback{rate, description, createdAt, id};
                   });
    return target;
}

std::pair<bool, std::vector<Feedback>> FeedbackReader::requestFeedbackBrowse()
{
    DBResult result;
    std::vector<DBEntry> entries;
    std::tie(result, entries) = DBProcessing::instance().requestTableData(DBTables::Feedbacks);
    return std::make_pair(result == DBResult::OK, transformFeedbacks(entries));
}

FeedbackReader::~FeedbackReader()
{
}
