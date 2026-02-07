#pragma once
#include <QString>
#include <QObject>

class NetworkClient;
class DBProcessing;

class UserRegistrationHandler : public QObject
{
    Q_OBJECT

public:
    explicit UserRegistrationHandler(NetworkClient* networkClient, QObject* parent = nullptr);

    void registerUser(const QString& login, const QString& password);
    QString validatePassword(const QString& password) const;
    QString hashPassword(const QString& password) const;

signals:
    void registrationStarted(const QString& login);
    void registrationSucceeded(int userId, const QString& login);
    void registrationFailed(const QString& error);

private slots:
    void onUserExistsCheckCompleted(bool exists, const QString& login);
    void onUserRegistered(int userId);
    void onNetworkError(const QString& error);

private:
    NetworkClient* m_networkClient;
    QString m_pendingLogin;
    QString m_pendingPassword;

    bool checkLocalUserExists(const QString& login) const;
};
