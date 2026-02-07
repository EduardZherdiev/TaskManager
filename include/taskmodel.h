#pragma once
#include <QAbstractListModel>
#include <QJsonArray>
#include "task.h"
#include "taskreader.h"

class UserModel;

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool showDeleted READ showDeleted WRITE setShowDeleted NOTIFY showDeletedChanged)
    Q_PROPERTY(int sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(int filterMonth READ filterMonth NOTIFY filterMonthChanged)
    Q_PROPERTY(int filterYear READ filterYear NOTIFY filterYearChanged)
    Q_PROPERTY(int archivedCount READ archivedCount NOTIFY statsChanged)
    Q_PROPERTY(int inProgressCount READ inProgressCount NOTIFY statsChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY statsChanged)
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

    QString lastError() const;
    
    int filterMonth() const;
    int filterYear() const;
    
    int archivedCount() const;
    int inProgressCount() const;
    int completedCount() const;
    
    void setUserModel(UserModel* userModel);

    Q_INVOKABLE bool createTask(const QString& title, const QString& description, int state);
    Q_INVOKABLE bool updateTask(int taskId, const QString& title, const QString& description, int state);
    Q_INVOKABLE bool softDeleteTask(int taskId);
    Q_INVOKABLE bool deleteTask(int taskId);
    Q_INVOKABLE bool restoreTask(int taskId);
    Q_INVOKABLE void reloadTasks();
    Q_INVOKABLE void setFilterMonth(int month, int year);

    void applyRemoteTasks(const QJsonArray &tasks);

signals:
    void showDeletedChanged();
    void sortFieldChanged();
    void statsChanged();
    void sortAscendingChanged();
    void lastErrorChanged();
    void filterMonthChanged();
    void filterYearChanged();

private slots:
    void onUserChanged();

private:
    std::vector<Task> m_Tasks;
    TaskReader m_reader;
    bool m_showDeleted = false;
    int m_sortField = 0;  // 0=CreatedAt, 1=Title, 2=State, 3=UpdatedAt, 4=DeletedAt
    bool m_sortAscending = true;
    int m_filterMonth = -1;  // -1 means no filter
    int m_archivedCount = 0;
    int m_inProgressCount = 0;
    int m_completedCount = 0;
    int m_filterYear = -1;
    QString m_lastError;
    UserModel* m_userModel = nullptr;
    void updateStats();

    int getCurrentUserId() const;
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
