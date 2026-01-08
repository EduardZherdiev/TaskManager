#include "taskmodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>
#include "database/dbprocessing.h"

bool TaskModel::showDeleted() const
{
    return m_showDeleted;
}

void TaskModel::setShowDeleted(bool show)
{
    if (m_showDeleted == show)
        return;
    
    m_showDeleted = show;
    emit showDeletedChanged();
    updateTask();
}

TaskModel::TaskModel()
{
    const bool updateResult {updateTask()};
    if (!updateResult) {
        qWarning() << "Update Task failed!";
    }
}

void TaskModel::registerMe(const std::string& moduleName)
{
    qmlRegisterType<TaskModel>(moduleName.c_str(), 1, 0, "TaskModel");
}

int TaskModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_Tasks.size());
}

QVariant TaskModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount(index)) {
        return {};
    }

    const Task& task = m_Tasks.at(index.row());

    switch (role) {
    case TaskRoles::IdRole:
        return QVariant::fromValue(static_cast<int>(task.id()));
    case TaskRoles::TitleRole:
        return QVariant::fromValue(task.title());
    case TaskRoles::DescriptionRole:
        return QVariant::fromValue(task.description());
    case TaskRoles::TaskStateRole:
        return QVariant::fromValue(static_cast<int>(task.taskState()));
    case TaskRoles::CreatedAtRole:
        return task.createdAt().isValid() ? QVariant::fromValue(task.createdAt().toString("dd.MM.yyyy HH:mm")) : QVariant();
    case TaskRoles::UpdatedAtRole:
        return task.updatedAt().isValid() ? QVariant::fromValue(task.updatedAt().toString("dd.MM.yyyy HH:mm")) : QVariant();
    case TaskRoles::DeletedAtRole:
        return task.deletedAt().isValid() ? QVariant::fromValue(task.deletedAt().toString("dd.MM.yyyy HH:mm")) : QVariant();
    default:
        return {};
    }
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TaskRoles::IdRole] = "id";
    roles[TaskRoles::TitleRole] = "title";
    roles[TaskRoles::DescriptionRole] = "description";
    roles[TaskRoles::TaskStateRole] = "taskState";
    roles[TaskRoles::CreatedAtRole] = "createdAt";
    roles[TaskRoles::UpdatedAtRole] = "updatedAt";
    roles[TaskRoles::DeletedAtRole] = "deletedAt";

    return roles;
}

bool TaskModel::updateTask()
{
    bool requestResult {false};
    std::vector<Task> TaskResult;
    std::tie(requestResult, TaskResult) = m_reader.requestTaskBrowse(m_showDeleted);

    if (requestResult) {
        beginResetModel();
        m_Tasks.swap(TaskResult);
        endResetModel();
    } else {
        qWarning() << "TaskModel::updateTask failed to load tasks";
    }

    return requestResult;
}

bool TaskModel::createTask(const QString& title, const QString& description, int state)
{
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }
    
    QVariantList data;
    data << defaultUserId()  // Valid UserId from Users table
         << title
         << description
         << state
         << QDateTime::currentDateTime()
         << QDateTime::currentDateTime()
         << QVariant();  // DeletedAt (null for new tasks)
    
    auto [result, taskId] = m_dbProcessor->requestAddRow(DBTypes::DBTables::Tasks, data);
    
    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task created with ID:" << taskId;
        updateTask();
        return true;
    }
    
    qWarning() << "Failed to create task";
    return false;
}

int TaskModel::defaultUserId()
{
    // Try to fetch first available user; if none, create a default one
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    std::vector<QVariantList> users;
    auto res = m_dbProcessor->requestTableData(DBTypes::DBTables::Users);
    if (res.first == DBTypes::DBResult::OK && !res.second.empty()) {
        const auto &row = res.second.front();
        // DBSelector layout: [rowid, Id, Login, PasswordHash]
        if (row.size() > 1)
            return row[1].toInt();
    }

    // No users, create a default one
    QVariantList data;
    data << QVariant("default") << QVariant("default_hash");
    auto add = m_dbProcessor->requestAddRow(DBTypes::DBTables::Users, data);
    return add.second;
}

bool TaskModel::updateTask(int taskId, const QString& title, const QString& description, int state)
{
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }
    
    QVector<QString> columns;
    columns << "Id" << "Title" << "Description" << "State" << "UpdatedAt";
    
    QVariantList values;
    values << taskId
           << title
           << description
           << state
           << QDateTime::currentDateTime();
    
    auto result = m_dbProcessor->requestUpdate(DBTypes::DBTables::Tasks, columns, values);
    
    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task updated with ID:" << taskId;
        this->updateTask();
        return true;
    }
    
    qWarning() << "Failed to update task";
    return false;
}

bool TaskModel::deleteTask(int taskId)
{
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }
    
    // Soft delete: set DeletedAt timestamp
    QVector<QString> columns;
    columns << "Id" << "DeletedAt";
    
    QVariantList values;
    values << taskId
           << QDateTime::currentDateTime();
    
    auto result = m_dbProcessor->requestUpdate(DBTypes::DBTables::Tasks, columns, values);
    
    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task deleted with ID:" << taskId;
        updateTask();
        return true;
    }
    
    qWarning() << "Failed to delete task";
    return false;
}
