#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>

class NetworkClient;

class UserUpdateHandler : public QObject
{
    Q_OBJECT

public:
    explicit UserUpdateHandler(NetworkClient* networkClient, QObject* parent = nullptr);

    void updateUser(int userId,
                    const QString& currentLogin,
                    const QString& newLogin,
                    const QString& oldPassword,
                    const QString& newPassword);

    QString validatePassword(const QString& password) const;

signals:
    void updateStarted(const QString& login);
    void updateSucceeded(const QString& login);
    void updateFailed(const QString& error);

private slots:
    void onUserExistsCheckCompleted(bool exists, const QString& login);
    void onUserUpdated(const QJsonObject& user);
    void onNetworkError(const QString& error);

private:
    void sendUpdateRequest();
    void resetPending();

    NetworkClient* m_networkClient = nullptr;
    int m_pendingUserId = -1;
    QString m_currentLogin;
    QString m_pendingLogin;
    QString m_pendingOldPassword;
    QString m_pendingNewPassword;
    bool m_waitingForLoginCheck = false;
    bool m_updateInProgress = false;
};
