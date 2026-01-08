#include "callbackmodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>

CallbackModel::CallbackModel()
{
    const bool updateResult {updateCallbacks()};
    if (!updateResult) {
        qWarning() << "Update Callbacks failed!";
    }
}

void CallbackModel::registerMe(const std::string& moduleName)
{
    qmlRegisterType<CallbackModel>(moduleName.c_str(), 1, 0, "CallbackModel");
}

int CallbackModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_Callbacks.size());
}

QVariant CallbackModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount(index)) {
        return {};
    }

    const Callback& cb = m_Callbacks.at(index.row());

    switch (role) {
    case CallbackRoles::IdRole:
        return QVariant::fromValue(static_cast<int>(cb.id()));
    case CallbackRoles::RateRole:
        return QVariant::fromValue(cb.rate());
    case CallbackRoles::DescriptionRole:
        return QVariant::fromValue(cb.description());
    case CallbackRoles::CreatedAtRole:
        return QVariant::fromValue(cb.createdAt().toString(Qt::ISODate));
    default:
        return {};
    }
}

QHash<int, QByteArray> CallbackModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CallbackRoles::IdRole] = "id";
    roles[CallbackRoles::RateRole] = "rate";
    roles[CallbackRoles::DescriptionRole] = "description";
    roles[CallbackRoles::CreatedAtRole] = "createdAt";
    return roles;
}

bool CallbackModel::updateCallbacks()
{
    bool requestResult {false};
    std::vector<Callback> result;
    std::tie(requestResult, result) = m_reader.requestCallbackBrowse();

    if (requestResult) {
        beginResetModel();
        m_Callbacks.swap(result);
        endResetModel();
    }

    return requestResult;
}
