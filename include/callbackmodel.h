#pragma once
#include <QAbstractListModel>
#include "callback.h"
#include "callbackreader.h"

class CallbackModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CallbackModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Callback> m_Callbacks;
    CallbackReader m_reader;

    enum CallbackRoles
    {
        IdRole = Qt::UserRole + 1,
        RateRole,
        DescriptionRole,
        CreatedAtRole
    };

    bool updateCallbacks();
};
