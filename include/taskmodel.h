#pragma once
#include <QAbstractListModel>
#include "task.h"
#include "taskreader.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool showDeleted READ showDeleted WRITE setShowDeleted NOTIFY showDeletedChanged)
    Q_PROPERTY(int sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)
    Q_PROPERTY(int currentUserId READ currentUserId NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserLogin READ currentUserLogin NOTIFY currentUserChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
    TaskModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool showDeleted() const;
    void setShowDeleted(bool show);

    int sortField() const;
    void setSortField(int field);

    bool sortAscending() const;
    void setSortAscending(bool ascending);

    int currentUserId() const;
    QString currentUserLogin() const;
    QString lastError() const;

    Q_INVOKABLE bool signIn(const QString& login, const QString& password);
    Q_INVOKABLE bool registerUser(const QString& login, const QString& password);
    Q_INVOKABLE void signOut();

    Q_INVOKABLE bool createTask(const QString& title, const QString& description, int state);
    Q_INVOKABLE bool updateTask(int taskId, const QString& title, const QString& description, int state);
    Q_INVOKABLE bool softDeleteTask(int taskId);
    Q_INVOKABLE bool deleteTask(int taskId);
    Q_INVOKABLE bool restoreTask(int taskId);
    Q_INVOKABLE void reloadTasks();

signals:
    void showDeletedChanged();
    void sortFieldChanged();
    void sortAscendingChanged();
    void currentUserChanged();
    void lastErrorChanged();

private:
    std::vector<Task> m_Tasks;
    TaskReader m_reader;
    bool m_showDeleted = false;
    int m_sortField = 0;  // 0=CreatedAt, 1=Title, 2=State, 3=UpdatedAt, 4=DeletedAt
    bool m_sortAscending = true;
    int m_currentUserId = -1;
    QString m_currentUserLogin;
    QString m_lastError;
    class DBProcessing* m_dbProcessor = nullptr;

    int defaultUserId();
    void sortTasks();
    void setError(const QString& err);

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

