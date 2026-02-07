#include "include/userregistrationhandler.h"
#include "network/networkclient.h"
#include "database/dbprocessing.h"
#include "database/dbtypes.h"
#include <QDebug>
#include <argon2.h>

UserRegistrationHandler::UserRegistrationHandler(NetworkClient* networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient)
{
    if (m_networkClient) {
        connect(m_networkClient, &NetworkClient::userExistsCheckCompleted,
                this, &UserRegistrationHandler::onUserExistsCheckCompleted);
        connect(m_networkClient, &NetworkClient::userRegistered,
                this, &UserRegistrationHandler::onUserRegistered);
        connect(m_networkClient, &NetworkClient::error,
                this, &UserRegistrationHandler::onNetworkError);
    }
}

void UserRegistrationHandler::registerUser(const QString& login, const QString& password)
{
    if (login.trimmed().isEmpty()) {
        emit registrationFailed(tr("Login cannot be empty"));
        return;
    }

    // Step 1: Validate password locally
    QString passwordError = validatePassword(password);
    if (!passwordError.isEmpty()) {
        emit registrationFailed(passwordError);
        return;
    }

    // Step 2: Check network connection availability
    if (!m_networkClient) {
        emit registrationFailed(tr("Network client not available. Please check your connection."));
        return;
    }

    // Save for later use
    m_pendingLogin = login;
    m_pendingPassword = password;
    emit registrationStarted(login);
    
    // Step 3: Check if login exists on server
    qDebug() << "Checking if login exists on server:" << login;
    m_networkClient->checkUserExists(login);
}

QString UserRegistrationHandler::validatePassword(const QString& password) const
{
    if (password.length() < 12) {
        return tr("Password must be at least 12 characters");
    }
    if (!password.contains(QRegularExpression("[a-z]"))) {
        return tr("Password must contain lowercase letters");
    }
    if (!password.contains(QRegularExpression("[A-Z]"))) {
        return tr("Password must contain uppercase letters");
    }
    if (!password.contains(QRegularExpression("[0-9]"))) {
        return tr("Password must contain digits");
    }
    if (!password.contains(QRegularExpression("[!@#$%^&*()]"))) {
        return tr("Password must contain special characters (!@#$%^&*())");
    }
    return "";
}

QString UserRegistrationHandler::hashPassword(const QString& password) const
{
    const uint32_t t_cost = 2;
    const uint32_t m_cost = (1 << 16);
    const uint32_t parallelism = 1;
    const size_t hashlen = 32;
    const size_t encodedlen = argon2_encodedlen(t_cost, m_cost, parallelism, 16, hashlen, Argon2_id);

    QByteArray passwordBytes = password.toUtf8();
    QByteArray salt = "TaskManagerSalt1";

    char encoded[encodedlen];
    int result = argon2id_hash_encoded(
        t_cost, m_cost, parallelism,
        passwordBytes.constData(), passwordBytes.size(),
        salt.constData(), salt.size(),
        hashlen, encoded, encodedlen
    );

    if (result != ARGON2_OK) {
        qWarning() << "Argon2 hashing failed:" << argon2_error_message(result);
        return QString();
    }

    return QString::fromUtf8(encoded);
}

bool UserRegistrationHandler::checkLocalUserExists(const QString& login) const
{
    QVariantList args;
    args << login;
    auto check = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Users,
                                                      "Login = ?", args);
    return check.first == DBTypes::DBResult::OK && !check.second.empty();
}

void UserRegistrationHandler::onUserExistsCheckCompleted(bool exists, const QString& login)
{
    qDebug() << "User exists check completed: login=" << login << "exists=" << exists;
    
    if (login != m_pendingLogin) {
        qDebug() << "Login mismatch, ignoring response";
        return;
    }

    // Step 3 result: Check if user exists on server
    if (exists) {
        emit registrationFailed(tr("User with this login already exists on server"));
        m_pendingLogin.clear();
        m_pendingPassword.clear();
        return;
    }

    // Step 4: Register user on server
    qDebug() << "Registering user on server:" << m_pendingLogin;
    m_networkClient->registerUser(m_pendingLogin, m_pendingPassword);
}

void UserRegistrationHandler::onUserRegistered(int userId)
{
    qDebug() << "User registered on server with userId:" << userId;
    
    if (userId <= 0) {
        emit registrationFailed(tr("Invalid user ID from server"));
        m_pendingLogin.clear();
        m_pendingPassword.clear();
        return;
    }

    // Step 5: Save to local database
    QString passwordHash = hashPassword(m_pendingPassword);
    QVariantList data;
    data << userId << m_pendingLogin << passwordHash;
    auto addResult = DBProcessing::instance().requestAddRow(DBTypes::DBTables::Users, data);
    
    if (addResult.first != DBTypes::DBResult::OK) {
        emit registrationFailed(tr("Failed to save user locally"));
        m_pendingLogin.clear();
        m_pendingPassword.clear();
        return;
    }

    qDebug() << "User saved locally. Registration succeeded!";
    emit registrationSucceeded(userId, m_pendingLogin);
    m_pendingLogin.clear();
    m_pendingPassword.clear();
}

void UserRegistrationHandler::onNetworkError(const QString& error)
{
    if (!m_pendingLogin.isEmpty()) {
        emit registrationFailed(error);
        m_pendingLogin.clear();
        m_pendingPassword.clear();
    }
}
