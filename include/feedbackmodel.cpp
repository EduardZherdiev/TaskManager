#include "Feedbackmodel.h"
#include "usermodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>
#include "database/dbprocessing.h"

FeedbackModel::FeedbackModel()
{
    // Don't load Feedbacks in constructor - wait for UserModel to be set
}

void FeedbackModel::setUserModel(UserModel* userModel)
{
    m_userModel = userModel;
}

QString FeedbackModel::lastError() const
{
    return m_lastError;
}

void FeedbackModel::setError(const QString& err)
{
    if (m_lastError == err)
        return;
    m_lastError = err;
    emit lastErrorChanged();
}

int FeedbackModel::getCurrentUserId() const
{
    if (m_userModel)
        return m_userModel->currentUserId();
    return -1;
}

void FeedbackModel::registerMe(const std::string& moduleName)
{
    qmlRegisterType<FeedbackModel>(moduleName.c_str(), 1, 0, "FeedbackModel");
}

int FeedbackModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_Feedbacks.size());
}

QVariant FeedbackModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount(index)) {
        return {};
    }

    const Feedback& cb = m_Feedbacks.at(index.row());

    switch (role) {
    case FeedbackRoles::IdRole:
        return QVariant::fromValue(static_cast<int>(cb.id()));
    case FeedbackRoles::RateRole:
        return QVariant::fromValue(cb.rate());
    case FeedbackRoles::DescriptionRole:
        return QVariant::fromValue(cb.description());
    case FeedbackRoles::CreatedAtRole:
        return QVariant::fromValue(cb.createdAt().toString(Qt::ISODate));
    default:
        return {};
    }
}

QHash<int, QByteArray> FeedbackModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FeedbackRoles::IdRole] = "id";
    roles[FeedbackRoles::RateRole] = "rate";
    roles[FeedbackRoles::DescriptionRole] = "description";
    roles[FeedbackRoles::CreatedAtRole] = "createdAt";
    return roles;
}

bool FeedbackModel::updateFeedbacks()
{
    bool requestResult {false};
    std::vector<Feedback> result;
    std::tie(requestResult, result) = m_reader.requestFeedbackBrowse();

    if (requestResult) {
        beginResetModel();
        m_Feedbacks.swap(result);
        endResetModel();
    }

    return requestResult;
}

bool FeedbackModel::hasUserFeedback()
{
    int userId = getCurrentUserId();
    
    if (userId < 0)
        return false;

    QVariantList args;
    args << userId;
    auto res = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Feedbacks,
                                                               "UserId = ?",
                                                               args);
    
    bool hasFeedback = res.first == DBTypes::DBResult::OK && !res.second.empty();
    
    return hasFeedback;
}

QVariantMap FeedbackModel::getUserFeedback()
{
    QVariantMap result;
    
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return result;
    }

    QVariantList args;
    args << userId;
    auto res = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Feedbacks,
                                                               "UserId = ?",
                                                               args);


    if (res.first != DBTypes::DBResult::OK || res.second.empty()) {
        return result;
    }

    const auto& row = res.second.front();
    // row layout: [Id, UserId, Rate, Description, CreatedAt]
    if (row.size() >= 5) {
        result["rate"] = row[3].toInt();
        result["description"] = row[4].toString();
        return result;
    }

    return result;
}

bool FeedbackModel::saveFeedback(int rate, const QString& description)
{
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
    }

    if (rate < 1 || rate > 5) {
        setError("Rating must be between 1 and 5");
        return false;
    }

    // Check if user already has feedback
    QVariantList args;
    args << userId;
    auto check = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Feedbacks,
                                                                 "UserId = ?",
                                                                 args);

    if (check.first == DBTypes::DBResult::OK && !check.second.empty()) {
        // Update existing feedback
        const auto& row = check.second.front();
        int feedbackId = row[1].toInt();

        QVector<QString> columns;
        columns << "Id" << "Rate" << "Description";

        QVariantList values;
        values << feedbackId << rate << description;

        auto result = DBProcessing::instance().requestUpdate(DBTypes::DBTables::Feedbacks, columns, values);
        
        if (result == DBTypes::DBResult::OK) {
            qDebug() << "Feedback updated for user" << userId;
            return true;
        }
        
        setError("Failed to update feedback");
        return false;
    } else {
        // Create new feedback
        QVariantList data;
        data << userId << rate << description << QDateTime::currentDateTime();

        auto [result, feedbackId] = DBProcessing::instance().requestAddRow(DBTypes::DBTables::Feedbacks, data);

        if (result == DBTypes::DBResult::OK) {
            qDebug() << "Feedback created with ID:" << feedbackId;
            return true;
        }

        setError("Failed to create feedback");
        return false;
    }
}

