#include "taskmodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>
#include <algorithm>
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

int TaskModel::sortField() const
{
    return m_sortField;
}

void TaskModel::setSortField(int field)
{
    if (m_sortField == field)
        return;
    
    m_sortField = field;
    emit sortFieldChanged();
    
    // Trigger view update
    beginResetModel();
    sortTasks();
    endResetModel();
}

bool TaskModel::sortAscending() const
{
    return m_sortAscending;
}

void TaskModel::setSortAscending(bool ascending)
{
    if (m_sortAscending == ascending)
        return;
    
    m_sortAscending = ascending;
    emit sortAscendingChanged();
    
    // Trigger view update
    beginResetModel();
    sortTasks();
    endResetModel();
}

int TaskModel::currentUserId() const
{
    return m_currentUserId;
}

QString TaskModel::currentUserLogin() const
{
    return m_currentUserLogin;
}

QString TaskModel::lastError() const
{
    return m_lastError;
}

void TaskModel::setError(const QString& err)
{
    if (m_lastError == err)
        return;
    m_lastError = err;
    emit lastErrorChanged();
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
    if (m_currentUserId < 0) {
        beginResetModel();
        m_Tasks.clear();
        endResetModel();
        return true;
    }

    bool requestResult {false};
    std::vector<Task> TaskResult;
    std::tie(requestResult, TaskResult) = m_reader.requestTaskBrowse(m_showDeleted, m_currentUserId);

    if (requestResult) {
        beginResetModel();
        m_Tasks.swap(TaskResult);
        sortTasks();
        endResetModel();
    } else {
        qWarning() << "TaskModel::updateTask failed to load tasks";
    }

    return requestResult;
}

void TaskModel::sortTasks()
{
    if (m_Tasks.empty())
        return;

    std::sort(m_Tasks.begin(), m_Tasks.end(), [this](const Task& a, const Task& b) {
        bool result = false;
        
        switch (m_sortField) {
        case 0: // CreatedAt
            result = a.createdAt() < b.createdAt();
            break;
        case 1: // Title
            result = a.title().toLower() < b.title().toLower();
            break;
        case 2: // State
            result = static_cast<int>(a.taskState()) < static_cast<int>(b.taskState());
            break;
        case 3: // UpdatedAt
            result = a.updatedAt() < b.updatedAt();
            break;
        case 4: // DeletedAt
            if (!a.deletedAt().isValid() && b.deletedAt().isValid())
                result = false;
            else if (a.deletedAt().isValid() && !b.deletedAt().isValid())
                result = true;
            else if (a.deletedAt().isValid() && b.deletedAt().isValid())
                result = a.deletedAt() < b.deletedAt();
            else
                result = false;
            break;
        default:
            result = a.createdAt() < b.createdAt();
        }
        
        return m_sortAscending ? result : !result;
    });
}

bool TaskModel::createTask(const QString& title, const QString& description, int state)
{
    if (m_currentUserId < 0) {
        setError("No user signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    QVariantList data;
    data << m_currentUserId  // Valid UserId from Users table
         << title
         << description
         << state
         << QDateTime::currentDateTime()
         << QVariant()
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

bool TaskModel::signIn(const QString& login, const QString& password)
{
    setError("");
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    QVariantList args;
    args << login << password;
    auto res = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                    "Login = ? AND PasswordHash = ?",
                                                    args);

    if (res.first != DBTypes::DBResult::OK || res.second.empty()) {
        setError("User not found or password is incorrect");
        return false;
    }

    const auto &row = res.second.front();
    if (row.size() < 2) {
        setError("Invalid user record");
        return false;
    }

    m_currentUserId = row[1].toInt();
    m_currentUserLogin = login;
    emit currentUserChanged();
    updateTask();
    return true;
}

bool TaskModel::registerUser(const QString& login, const QString& password)
{
    setError("");
    if (login.trimmed().isEmpty()) {
        setError("Login cannot be empty");
        return false;
    }
    if (password.isEmpty()) {
        setError("Password cannot be empty");
        return false;
    }
    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    // Check if login exists
    QVariantList args;
    args << login;
    auto check = m_dbProcessor->requestTableDataWhere(DBTypes::DBTables::Users,
                                                      "Login = ?",
                                                      args);
    if (check.first == DBTypes::DBResult::OK && !check.second.empty()) {
        setError("User already exists");
        return false;
    }

    QVariantList data;
    data << login << password;
    auto add = m_dbProcessor->requestAddRow(DBTypes::DBTables::Users, data);
    if (add.first != DBTypes::DBResult::OK) {
        setError("Failed to register user");
        return false;
    }

    // Auto sign-in
    m_currentUserId = add.second;
    m_currentUserLogin = login;
    emit currentUserChanged();
    updateTask();
    return true;
}

void TaskModel::signOut()
{
    m_currentUserId = -1;
    m_currentUserLogin.clear();
    emit currentUserChanged();
    updateTask();
}

bool TaskModel::updateTask(int taskId, const QString& title, const QString& description, int state)
{
    if (m_currentUserId < 0) {
        setError("No user signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    const Task* existingTask = nullptr;
    for (const auto& t : m_Tasks) {
        if (t.id() == taskId) {
            existingTask = &t;
            break;
        }
    }

    QVector<QString> columns;
    QVariantList values;

    columns << "Id";
    values << taskId;

    bool changed = false;
    if (existingTask) {
        if (title != existingTask->title()) {
            columns << "Title";
            values << title;
            changed = true;
        }
        if (description != existingTask->description()) {
            columns << "Description";
            values << description;
            changed = true;
        }
        if (state != static_cast<int>(existingTask->taskState())) {
            columns << "State";
            values << state;
            changed = true;
        }
    } else {
        // Fallback: update everything if we can't find the task in memory
        columns << "Title" << "Description" << "State";
        values << title << description << state;
        changed = true;
    }

    if (!changed) {
        return true; // nothing to update
    }

    columns << "UpdatedAt";
    values << QDateTime::currentDateTime();

    auto result = m_dbProcessor->requestUpdate(DBTypes::DBTables::Tasks, columns, values);

    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task updated with ID:" << taskId;
        this->updateTask();
        return true;
    }
    
    qWarning() << "Failed to update task";
    return false;
}

bool TaskModel::softDeleteTask(int taskId)
{
    if (m_currentUserId < 0) {
        setError("No user signed in");
        return false;
    }
    
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
        qDebug() << "Task soft deleted with ID:" << taskId;
        updateTask();
        return true;
    }
    
    qWarning() << "Failed to soft delete task";
    return false;
}

bool TaskModel::deleteTask(int taskId)
{
    if (m_currentUserId < 0) {
        setError("No user signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    auto result = m_dbProcessor->requestDelete(DBTypes::DBTables::Tasks,taskId);

    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task deleted with ID:" << taskId;
        updateTask();
        return true;
    }

    qWarning() << "Failed to delete task";
    return false;
}

bool TaskModel::restoreTask(int taskId)
{
    if (m_currentUserId < 0) {
        setError("No user signed in");
        return false;
    }

    if (!m_dbProcessor) {
        m_dbProcessor = new DBProcessing();
    }

    // Remove DeletedAt (set NULL) and reset state to Active (0)
    QVector<QString> columns;
    columns << "Id" << "DeletedAt";

    QVariantList values;
    values << taskId
           << QVariant()  ;     // State -> Active

    auto result = m_dbProcessor->requestUpdate(DBTypes::DBTables::Tasks, columns, values);

    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task restored with ID:" << taskId;
        updateTask();
        return true;
    }

    qWarning() << "Failed to restore task";
    return false;
}

void TaskModel::reloadTasks()
{
    qDebug() << "Reloading tasks from database";
    updateTask();
}
