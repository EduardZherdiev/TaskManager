#include "include/usermodel.h"
#include "include/userregistrationhandler.h"
#include "network/networkclient.h"
#include <QQmlEngine>
#include <QDebug>
#include <argon2.h>
#include <QRandomGenerator>
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

QString UserModel::hashPassword(const QString& password)
{
    // Argon2id parameters
    const uint32_t t_cost = 2;            // 2 iterations
    const uint32_t m_cost = (1 << 16);    // 64 MiB memory usage
    const uint32_t parallelism = 1;       // single-threaded
    const size_t hashlen = 32;            // 32 bytes output
    static const size_t encodedlen = argon2_encodedlen(t_cost, m_cost, parallelism, 16, hashlen, Argon2_id);
    static const QByteArray salt("TaskManagerSalt1"); // Fixed salt (for production use random salt per user)

    QByteArray passwordBytes = password.toUtf8();
    
    QByteArray encoded(static_cast<int>(encodedlen), '\0');
    
    int result = argon2id_hash_encoded(
        t_cost,
        m_cost,
        parallelism,
        passwordBytes.constData(),
        passwordBytes.size(),
        salt.constData(),
        salt.size(),
        hashlen,
        encoded.data(),
        encoded.size()
    );
    
    if (result != ARGON2_OK) {
        qWarning() << "Argon2 hashing failed:" << argon2_error_message(result);
        return QString();
    }
    
    return QString::fromUtf8(encoded.constData());
}

bool UserModel::signIn(const QString& login, const QString& password)
{
    qDebug() << "UserModel::signIn called with login=" << login;
    setError("");

    QString passwordHash = hashPassword(password);
    qDebug() << "Password hash:" << passwordHash;
    QVariantList args;
    args << login << passwordHash;
    auto res = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                    "Login = ? AND PasswordHash = ?",
                                                    args);

    qDebug() << "Query result:" << (int)res.first << "rows found:" << res.second.size();
    
    if (res.first != DBTypes::DBResult::OK || res.second.empty()) {
        setError(tr("User not found or password is incorrect"));
        qDebug() << "Sign in failed:" << m_lastError;
        return false;
    }

    const auto &row = res.second.front();
    if (row.size() < 3) {
        setError(tr("Invalid user record"));
        return false;
    }

    m_currentUserId = row[0].toInt();
    m_currentUserLogin = login;
    qDebug() << "Sign in successful! userId=" << m_currentUserId;
    emit currentUserChanged();
    return true;
}

bool UserModel::signInWithHash(const QString& login, const QString& passwordHash)
{
    setError("");

    QVariantList args;
    args << login << passwordHash;
    auto res = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                    "Login = ? AND PasswordHash = ?",
                                                    args);

    if (res.first != DBTypes::DBResult::OK || res.second.empty()) {
        setError(tr("User not found or password is incorrect"));
        return false;
    }

    const auto &row = res.second.front();
    if (row.size() < 3) {
        setError(tr("Invalid user record"));
        return false;
    }

    m_currentUserId = row[0].toInt();
    m_currentUserLogin = login;
    emit currentUserChanged();
    return true;
}

bool UserModel::registerUser(const QString& login, const QString& password)
{
    setError("");
    if (login.trimmed().isEmpty()) {
        setError(tr("Login cannot be empty"));
        return false;
    }
    
    // Validate password
    QString passwordError = validatePassword(password);
    if (!passwordError.isEmpty()) {
        setError(passwordError);
        return false;
    }

    // Check server connection by testing health
    if (!m_networkClient) {
        setError(tr("Network client not available. Please check your connection."));
        return false;
    }

    // Check if login exists in local database
    QVariantList args;
    args << login;
    auto localCheck = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                      "Login = ?",
                                                      args);
    if (localCheck.first == DBTypes::DBResult::OK && !localCheck.second.empty()) {
        setError(tr("User already exists locally"));
        return false;
    }

    // Hash password
    QString passwordHash = hashPassword(password);
    
    // Add to local database
    QVariantList data;
    data << QVariant() << login << passwordHash;
    auto add = DBProcessing::instance().requestAddRow(DBTypes::DBTables::Users, data);
    if (add.first != DBTypes::DBResult::OK) {
        setError(tr("Failed to register user locally"));
        return false;
    }

    // Auto sign-in
    m_currentUserId = add.second;
    m_currentUserLogin = login;
    emit currentUserChanged();
    
    // Send to server asynchronously (fire and forget for now)
    // In future, this should be handled by UserRegistrationHandler
    m_networkClient->registerUser(login, password);
    
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
        setError(tr("Not signed in"));
        return false;
    }

    QVariantList args;
    args << password;
    // First get the current user's record
    QVariantList userArgs;
    userArgs << m_currentUserId;
    auto userRes = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                        "Id = ?",
                                                        userArgs);
    if (userRes.first != DBTypes::DBResult::OK || userRes.second.empty()) {
        setError(tr("User not found"));
        return false;
    }

    const auto &userRow = userRes.second.front();
    if (userRow.size() < 3) {
        setError(tr("Invalid user record"));
        return false;
    }

    QString storedPasswordHash = userRow[3].toString();
    QString passwordHash = hashPassword(password);
    if (storedPasswordHash != passwordHash) {
        setError(tr("Current password is incorrect"));
        return false;
    }

    return true;
}

bool UserModel::updateUser(const QString& newLogin, const QString& oldPassword, const QString& newPassword)
{
    setError("");
    if (m_currentUserId < 0) {
        setError(tr("Not signed in"));
        return false;
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
        auto check = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                          "Login = ?",
                                                          checkArgs);
        if (check.first == DBTypes::DBResult::OK && !check.second.empty()) {
            const auto &row = check.second.front();
            if (row.size() >= 2 && row[1].toInt() != m_currentUserId) {
                setError(tr("Login already taken"));
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
        QString newPasswordHash = hashPassword(newPassword);
        columns << "PasswordHash";
        values << newPasswordHash;
    } else {
        // Keep old password - get it from database
        QVariantList getPasswordArgs;
        getPasswordArgs << m_currentUserId;
        auto passRes = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                           "Id = ?",
                                                           getPasswordArgs);
        if (passRes.first != DBTypes::DBResult::OK || passRes.second.empty()) {
            setError(tr("Failed to retrieve user data"));
            return false;
        }
        const auto &row = passRes.second.front();
        if (row.size() < 3) {
            setError(tr("Invalid user record"));
            return false;
        }
        QString currentPassword = row[2].toString();
        columns << "PasswordHash";
        values << currentPassword;
    }

    auto result = DBProcessing::instance().requestUpdate(DBTypes::DBTables::Users, columns, values);
    if (result != DBTypes::DBResult::OK) {
        setError(tr("Failed to update user"));
        return false;
    }

    m_currentUserLogin = newLogin;
    emit currentUserChanged();
    updateUsers();
    return true;
}

QString UserModel::validatePassword(const QString& password)
{
    // Minimum 12 characters
    if (password.length() < 12) {
        return tr("Password must be at least 12 characters long");
    }

    bool hasLower = false;
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    const QString specialChars = "!@#$%^&*()";

    for (const QChar& c : password) {
        if (c.isLower()) hasLower = true;
        else if (c.isUpper()) hasUpper = true;
        else if (c.isDigit()) hasDigit = true;
        else if (specialChars.contains(c)) hasSpecial = true;
    }

    if (!hasLower) {
        return tr("Password must contain lowercase letters (a-z)");
    }
    if (!hasUpper) {
        return tr("Password must contain uppercase letters (A-Z)");
    }
    if (!hasDigit) {
        return tr("Password must contain digits (0-9)");
    }
    if (!hasSpecial) {
        return tr("Password must contain special characters (!@#$%^&*())");
    }

    return ""; // Empty string means password is valid
}

QString UserModel::generatePassword()
{
    const QString lowercase = "abcdefghijklmnopqrstuvwxyz";
    const QString uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const QString digits = "0123456789";
    const QString special = "!@#$%^&*()";

    QRandomGenerator rng(QRandomGenerator::securelySeeded());
    QString password;

    // Add at least one of each required character type
    password += lowercase[rng.bounded(lowercase.length())];
    password += uppercase[rng.bounded(uppercase.length())];
    password += digits[rng.bounded(digits.length())];
    password += special[rng.bounded(special.length())];

    // Fill remaining 8 characters randomly from all character types
    QString allChars = lowercase + uppercase + digits + special;
    for (int i = 0; i < 8; ++i) {
        password += allChars[rng.bounded(allChars.length())];
    }

    // Shuffle password
    for (int i = password.length() - 1; i > 0; --i) {
        int j = rng.bounded(i + 1);
        std::swap(password[i], password[j]);
    }

    return password;
}

void UserModel::setNetworkClient(NetworkClient* client)
{
    m_networkClient = client;
}

void UserModel::setRegistrationHandler(UserRegistrationHandler* handler)
{
    m_registrationHandler = handler;
    if (m_registrationHandler) {
        connect(m_registrationHandler, &UserRegistrationHandler::registrationSucceeded,
                this, [this](int userId, const QString& login) {
            // Auto sign-in the user after successful registration
            m_currentUserId = userId;
            m_currentUserLogin = login;
            emit currentUserChanged();
            emit serverRegistrationSucceeded(userId);
        });
        connect(m_registrationHandler, &UserRegistrationHandler::registrationFailed,
                this, [this](const QString& error) {
            setError(error);
            emit serverRegistrationFailed(error);
        });
    }
}

void UserModel::registerUserWithServer(const QString& login, const QString& password)
{
    if (!m_registrationHandler) {
        setError(tr("Registration handler not initialized"));
        emit serverRegistrationFailed(tr("Registration handler not initialized"));
        return;
    }
    
    m_registrationHandler->registerUser(login, password);
}

