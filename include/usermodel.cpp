#include "usermodel.h"
#include <QQmlEngine>
#include <QDebug>
#include "database/dbprocessing.h"
#include "database/dbtypes.h"

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

int UserModel::currentUserId() const
{
    return m_currentUserId;
}

QString UserModel::currentUserLogin() const
{
    return m_currentUserLogin;
}

QString UserModel::lastError() const
{
    return m_lastError;
}

void UserModel::setError(const QString& err)
{
    if (m_lastError != err) {
        m_lastError = err;
        emit lastErrorChanged();
    }
}

bool UserModel::signIn(const QString& login, const QString& password)
{
    setError("");
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    QVariantList args;
    args << login << password;
    auto res = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                    "Login = ? AND PasswordHash = ?",
                                                    args);

    if (res.first != DBTypes::DBResult::OK || res.second.empty()) {
        setError("User not found or password is incorrect");
        return false;
    }

    const auto &row = res.second.front();
    if (row.size() < 2) {
        setError("Invalid user record");
        return false;
    }

    m_currentUserId = row[1].toInt();
    m_currentUserLogin = login;
    emit currentUserChanged();
    return true;
}

bool UserModel::registerUser(const QString& login, const QString& password)
{
    setError("");
    if (login.trimmed().isEmpty()) {
        setError("Login cannot be empty");
        return false;
    }
    if (password.isEmpty()) {
        setError("Password cannot be empty");
        return false;
    }
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    // Check if login exists
    QVariantList args;
    args << login;
    auto check = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                      "Login = ?",
                                                      args);
    if (check.first == DBTypes::DBResult::OK && !check.second.empty()) {
        setError("User already exists");
        return false;
    }

    QVariantList data;
    data << login << password;
    auto add = m_dbProcessor->requestAddRow(DBTypes::DBTables::Users, data);
    if (add.first != DBTypes::DBResult::OK) {
        setError("Failed to register user");
        return false;
    }

    // Auto sign-in
    m_currentUserId = add.second;
    m_currentUserLogin = login;
    emit currentUserChanged();
    return true;
}

void UserModel::signOut()
{
    m_currentUserId = -1;
    m_currentUserLogin.clear();
    emit currentUserChanged();
}

bool UserModel::checkPassword(const QString& password)
{
    setError("");
    if (m_currentUserId < 0) {
        setError("Not signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    QVariantList args;
    args << password;
    // First get the current user's record
    QVariantList userArgs;
    userArgs << m_currentUserId;
    auto userRes = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                        "Id = ?",
                                                        userArgs);
    if (userRes.first != DBTypes::DBResult::OK || userRes.second.empty()) {
        setError("User not found");
        return false;
    }

    const auto &userRow = userRes.second.front();
    if (userRow.size() < 3) {
        setError("Invalid user record");
        return false;
    }

    QString storedPassword = userRow[3].toString();
    if (storedPassword != password) {
        setError("Current password is incorrect");
        return false;
    }

    return true;
}

bool UserModel::updateUser(const QString& newLogin, const QString& oldPassword, const QString& newPassword)
{
    setError("");
    if (m_currentUserId < 0) {
        setError("Not signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    // If password is being changed, verify old password
    if (!newPassword.isEmpty()) {
        if (!checkPassword(oldPassword)) {
            return false;
        }
    }

    // Check if new login is already taken by another user
    if (newLogin != m_currentUserLogin) {
        QVariantList checkArgs;
        checkArgs << newLogin;
        auto check = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                          "Login = ?",
                                                          checkArgs);
        if (check.first == DBTypes::DBResult::OK && !check.second.empty()) {
            const auto &row = check.second.front();
            if (row.size() >= 2 && row[1].toInt() != m_currentUserId) {
                setError("Login already taken");
                return false;
            }
        }
    }

    // Update user data
    QVector<QString> columns;
    QVariantList values;
    
    columns << "Id";
    values << m_currentUserId;
    
    columns << "Login";
    values << newLogin;
    
    if (!newPassword.isEmpty()) {
        columns << "PasswordHash";
        values << newPassword;
    } else {
        // Keep old password - get it from database
        QVariantList getPasswordArgs;
        getPasswordArgs << m_currentUserId;
        auto passRes = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                           "Id = ?",
                                                           getPasswordArgs);
        if (passRes.first != DBTypes::DBResult::OK || passRes.second.empty()) {
            setError("Failed to retrieve user data");
            return false;
        }
        const auto &row = passRes.second.front();
        if (row.size() < 3) {
            setError("Invalid user record");
            return false;
        }
        QString currentPassword = row[2].toString();
        columns << "PasswordHash";
        values << currentPassword;
    }

    auto result = m_dbProcessor->requestUpdate(DBTypes::DBTables::Users, columns, values);
    if (result != DBTypes::DBResult::OK) {
        setError("Failed to update user");
        return false;
    }

    m_currentUserLogin = newLogin;
    emit currentUserChanged();
    updateUsers();
    return true;
}
