#pragma once
#include <QAbstractListModel>
#include "user.h"
#include "userreader.h"

class UserRegistrationHandler;
class UserUpdateHandler;

class UserModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentUserId READ currentUserId NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserLogin READ currentUserLogin NOTIFY currentUserChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
    UserModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentUserId() const;
    QString currentUserLogin() const;
    QString lastError() const;

    Q_INVOKABLE bool signIn(const QString& login, const QString& password);
    Q_INVOKABLE bool signInWithHash(const QString& login, const QString& passwordHash);
    Q_INVOKABLE bool registerUser(const QString& login, const QString& password);
    Q_INVOKABLE void registerUserWithServer(const QString& login, const QString& password);
    Q_INVOKABLE void signOut();
    Q_INVOKABLE bool checkPassword(const QString& password);
    Q_INVOKABLE bool updateUser(const QString& newLogin, const QString& oldPassword, const QString& newPassword);
    Q_INVOKABLE void updateUserWithServer(const QString& newLogin, const QString& oldPassword, const QString& newPassword);
    Q_INVOKABLE QString hashPassword(const QString& password);
    Q_INVOKABLE QString validatePassword(const QString& password);
    Q_INVOKABLE QString generatePassword();
    
    void setNetworkClient(class NetworkClient* client);
    void setRegistrationHandler(class UserRegistrationHandler* handler);
    void setUpdateHandler(class UserUpdateHandler* handler);

signals:
    void currentUserChanged();
    void lastErrorChanged();
    void serverRegistrationSucceeded(int userId);
    void serverRegistrationFailed(const QString& error);
    void serverUserUpdateSucceeded(const QString& login);
    void serverUserUpdateFailed(const QString& error);

private:
    std::vector<User> m_Users;
    UserReader m_reader;
    int m_currentUserId = -1;
    QString m_currentUserLogin;
    QString m_lastError;
    UserRegistrationHandler* m_registrationHandler = nullptr;
    UserUpdateHandler* m_updateHandler = nullptr;
    class NetworkClient* m_networkClient = nullptr;
    QString m_pendingUpdateLogin;
    QString m_pendingUpdatePassword;

    enum UserRoles
    {
        IdRole = Qt::UserRole + 1,
        LoginRole,
        PasswordHashRole
    };

    bool updateUsers();
    bool applyLocalUserUpdate(const QString& newLogin, const QString& newPassword);
    void setError(const QString& err);
};
