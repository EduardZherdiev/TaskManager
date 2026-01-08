#include "usermodel.h"
#include <QQmlEngine>
#include <QDebug>

UserModel::UserModel()
{
    const bool updateResult {updateUsers()};
    if (!updateResult) {
        qWarning() << "Update Users failed!";
    }
}

void UserModel::registerMe(const std::string& moduleName)
{
    qmlRegisterType<UserModel>(moduleName.c_str(), 1, 0, "UserModel");
}

int UserModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_Users.size());
}

QVariant UserModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount(index)) {
        return {};
    }

    const User& user = m_Users.at(index.row());

    switch (role) {
    case UserRoles::IdRole:
        return QVariant::fromValue(static_cast<int>(user.id()));
    case UserRoles::LoginRole:
        return QVariant::fromValue(user.login());
    case UserRoles::PasswordHashRole:
        return QVariant::fromValue(user.passwordHash());
    default:
        return {};
    }
}

QHash<int, QByteArray> UserModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UserRoles::IdRole] = "id";
    roles[UserRoles::LoginRole] = "login";
    roles[UserRoles::PasswordHashRole] = "passwordHash";
    return roles;
}

bool UserModel::updateUsers()
{
    bool requestResult {false};
    std::vector<User> result;
    std::tie(requestResult, result) = m_reader.requestUserBrowse();

    if (requestResult) {
        beginResetModel();
        m_Users.swap(result);
        endResetModel();
    }

    return requestResult;
}
