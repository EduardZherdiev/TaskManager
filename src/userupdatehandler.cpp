#include "include/userupdatehandler.h"
#include "network/networkclient.h"
#include <QRegularExpression>
#include <QDebug>

UserUpdateHandler::UserUpdateHandler(NetworkClient* networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient)
{
    if (m_networkClient) {
        connect(m_networkClient, &NetworkClient::userExistsCheckCompleted,
                this, &UserUpdateHandler::onUserExistsCheckCompleted);
        connect(m_networkClient, &NetworkClient::userUpdated,
                this, &UserUpdateHandler::onUserUpdated);
        connect(m_networkClient, &NetworkClient::error,
                this, &UserUpdateHandler::onNetworkError);
    }
}

void UserUpdateHandler::updateUser(int userId,
                                  const QString& currentLogin,
                                  const QString& newLogin,
                                  const QString& oldPassword,
                                  const QString& newPassword)
{
    if (!m_networkClient) {
        emit updateFailed(tr("Network client not available. Please check your connection."));
        return;
    }

    const QString trimmedLogin = newLogin.trimmed();
    if (userId <= 0) {
        emit updateFailed(tr("Not signed in"));
        return;
    }
    if (trimmedLogin.isEmpty()) {
        emit updateFailed(tr("Login cannot be empty"));
        return;
    }

    if (!newPassword.isEmpty()) {
        if (oldPassword.isEmpty()) {
            emit updateFailed(tr("Please enter current password"));
            return;
        }
        const QString passwordError = validatePassword(newPassword);
        if (!passwordError.isEmpty()) {
            emit updateFailed(passwordError);
            return;
        }
    }

    if (trimmedLogin == currentLogin && newPassword.isEmpty()) {
        emit updateFailed(tr("No changes to update"));
        return;
    }

    m_pendingUserId = userId;
    m_currentLogin = currentLogin;
    m_pendingLogin = trimmedLogin;
    m_pendingOldPassword = oldPassword;
    m_pendingNewPassword = newPassword;

    emit updateStarted(trimmedLogin);

    if (trimmedLogin != currentLogin) {
        m_waitingForLoginCheck = true;
        m_networkClient->checkUserExists(trimmedLogin);
        return;
    }

    sendUpdateRequest();
}

QString UserUpdateHandler::validatePassword(const QString& password) const
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

void UserUpdateHandler::onUserExistsCheckCompleted(bool exists, const QString& login)
{
    if (!m_waitingForLoginCheck || login != m_pendingLogin) {
        return;
    }

    m_waitingForLoginCheck = false;

    if (exists) {
        emit updateFailed(tr("Login already taken on server"));
        resetPending();
        return;
    }

    sendUpdateRequest();
}

void UserUpdateHandler::sendUpdateRequest()
{
    if (!m_networkClient) {
        emit updateFailed(tr("Network client not available. Please check your connection."));
        resetPending();
        return;
    }

    m_updateInProgress = true;
    m_networkClient->updateCurrentUser(m_pendingLogin, m_pendingOldPassword, m_pendingNewPassword);
}

void UserUpdateHandler::onUserUpdated(const QJsonObject& user)
{
    if (!m_updateInProgress) {
        return;
    }

    m_updateInProgress = false;
    QString login = user.value("login").toString();
    if (login.isEmpty()) {
        login = m_pendingLogin;
    }

    emit updateSucceeded(login);
    resetPending();
}

void UserUpdateHandler::onNetworkError(const QString& error)
{
    if (!m_waitingForLoginCheck && !m_updateInProgress) {
        return;
    }

    emit updateFailed(error);
    resetPending();
}

void UserUpdateHandler::resetPending()
{
    m_pendingUserId = -1;
    m_currentLogin.clear();
    m_pendingLogin.clear();
    m_pendingOldPassword.clear();
    m_pendingNewPassword.clear();
    m_waitingForLoginCheck = false;
    m_updateInProgress = false;
}
