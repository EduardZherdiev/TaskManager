#pragma once
#include <QAbstractListModel>
#include <QCryptographicHash>
#include "user.h"
#include "userreader.h"

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
    Q_INVOKABLE void signOut();
    Q_INVOKABLE bool checkPassword(const QString& password);
    Q_INVOKABLE bool updateUser(const QString& newLogin, const QString& oldPassword, const QString& newPassword);
    Q_INVOKABLE QString hashPassword(const QString& password);

signals:
    void currentUserChanged();
    void lastErrorChanged();

private:
    std::vector<User> m_Users;
    UserReader m_reader;
    int m_currentUserId = -1;
    QString m_currentUserLogin;
    QString m_lastError;

    enum UserRoles
    {
        IdRole = Qt::UserRole + 1,
        LoginRole,
        PasswordHashRole
    };

    bool updateUsers();
    void setError(const QString& err);
};
