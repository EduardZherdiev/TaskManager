#pragma once
#include <QAbstractListModel>
#include "task.h"
#include "taskreader.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool showDeleted READ showDeleted WRITE setShowDeleted NOTIFY showDeletedChanged)
public:
    TaskModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool showDeleted() const;
    void setShowDeleted(bool show);

    Q_INVOKABLE bool createTask(const QString& title, const QString& description, int state);
    Q_INVOKABLE bool updateTask(int taskId, const QString& title, const QString& description, int state);
    Q_INVOKABLE bool deleteTask(int taskId);
    Q_INVOKABLE bool restoreTask(int taskId);

signals:
    void showDeletedChanged();

private:
    std::vector<Task> m_Tasks;
    TaskReader m_reader;
    bool m_showDeleted = false;
    class DBProcessing* m_dbProcessor = nullptr;

    int defaultUserId();

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

