#pragma once
#include <QAbstractListModel>
#include "task.h"
#include "taskreader.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TaskModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Task> m_Tasks;
    TaskReader m_reader;

    enum TaskRoles
    {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        TaskStateRole,
        CreatedAtRole,
        UpdatedAtRole,
        DeletedAtRole
    };

    bool updateTask();
};

