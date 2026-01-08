#pragma once
#include <QAbstractListModel>
#include "user.h"
#include "userreader.h"

class UserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    UserModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<User> m_Users;
    UserReader m_reader;

    enum UserRoles
    {
        IdRole = Qt::UserRole + 1,
        LoginRole,
        PasswordHashRole
    };

    bool updateUsers();
};
