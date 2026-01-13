#include "include/taskmodel.h"
#include "include/usermodel.h"
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
    
    qDebug() << "Changing sort field from" << m_sortField << "to" << field;
    m_sortField = field;
    emit sortFieldChanged();
    
    // Trigger view update
    qDebug() << "Triggering view update after sort field change";
    beginResetModel();
    sortTasks();
    endResetModel();
    qDebug() << "View update completed";
}

bool TaskModel::sortAscending() const
{
    return m_sortAscending;
}

void TaskModel::setSortAscending(bool ascending)
{
    if (m_sortAscending == ascending)
        return;
    
    qDebug() << "Changing sort order to" << (ascending ? "ascending" : "descending");
    m_sortAscending = ascending;
    emit sortAscendingChanged();
    
    // Trigger view update
    qDebug() << "Triggering view update after sort order change";
    beginResetModel();
    sortTasks();
    endResetModel();
    qDebug() << "View update completed";
}

QString TaskModel::lastError() const
{
    return m_lastError;
}

int TaskModel::getCurrentUserId() const
{
    // Get userId from UserModel
    if (m_userModel)
        return m_userModel->currentUserId();
    return -1;
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
    // UserModel will be set via setUserModel() from main.cpp
}

void TaskModel::setUserModel(UserModel* userModel)
{
    m_userModel = userModel;
    if (m_userModel) {
        // Connect to UserModel's currentUserChanged signal
        connect(m_userModel, SIGNAL(currentUserChanged()), this, SLOT(onUserChanged()));
        // Don't call updateTask here - it will be called when currentUserChanged signal is emitted
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
    int userId = getCurrentUserId();
    if (userId < 0) {
        beginResetModel();
        m_Tasks.clear();
        endResetModel();
        return true;
    }

    qDebug() << "Loading tasks from database...";
    bool requestResult {false};
    std::vector<Task> TaskResult;
    std::tie(requestResult, TaskResult) = m_reader.requestTaskBrowse(m_showDeleted, userId);
    qDebug() << "Tasks loaded, count:" << TaskResult.size();
    
    // Apply month filter if set
    if (m_filterMonth >= 0 && m_filterYear >= 0) {
        TaskResult.erase(
            std::remove_if(TaskResult.begin(), TaskResult.end(), 
                [this](const Task& task) {
                    QDate date = task.createdAt().date();
                    return date.month() - 1 != m_filterMonth || date.year() != m_filterYear;
                }),
            TaskResult.end()
        );
        qDebug() << "Tasks after month filter:" << TaskResult.size();
    }
    
    if (requestResult) {
        qDebug() << "Starting model update...";
        beginResetModel();
        m_Tasks.swap(TaskResult);
        qDebug() << "Starting sort...";
        sortTasks();
        qDebug() << "Sort completed";
        endResetModel();
        qDebug() << "Model update completed";
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
        int comparison = 0;
        
        switch (m_sortField) {
        case 0: // CreatedAt
        {
            bool aValid = a.createdAt().isValid();
            bool bValid = b.createdAt().isValid();
            if (!aValid && !bValid)
                comparison = 0; // Both invalid, equal
            else if (!aValid)
                comparison = -1; // Invalid comes after valid (always at end)
            else if (!bValid)
                comparison = 1; // Valid comes before invalid
            else if (a.createdAt() < b.createdAt())
                comparison = -1;
            else if (a.createdAt() > b.createdAt())
                comparison = 1;
        }
            break;
        case 1: // Title
            comparison = QString::compare(a.title(), b.title(), Qt::CaseInsensitive);
            break;
        case 2: // State
            {
                int stateA = static_cast<int>(a.taskState());
                int stateB = static_cast<int>(b.taskState());
                if (stateA < stateB)
                    comparison = -1;
                else if (stateA > stateB)
                    comparison = 1;
            }
            break;
        case 3: // UpdatedAt
            {
                bool aValid = a.updatedAt().isValid();
                bool bValid = b.updatedAt().isValid();
                if (!aValid && !bValid)
                    comparison = 0; // Both invalid, equal
                else if (!aValid)
                    comparison = -1; // Invalid comes after valid (always at end)
                else if (!bValid)
                    comparison = 1; // Valid comes before invalid
                else if (a.updatedAt() < b.updatedAt())
                    comparison = -1;
                else if (a.updatedAt() > b.updatedAt())
                    comparison = 1;
            }
            break;
        case 4: // DeletedAt
            {
                bool aValid = a.deletedAt().isValid();
                bool bValid = b.deletedAt().isValid();
                if (!aValid && !bValid)
                    comparison = 0;
                else if (!aValid)
                    comparison = 1; // Invalid comes after valid (always at end)
                else if (!bValid)
                    comparison = -1; // Valid comes before invalid
                else if (a.deletedAt() < b.deletedAt())
                    comparison = -1;
                else if (a.deletedAt() > b.deletedAt())
                    comparison = 1;
            }
            break;
        default:
            if (a.createdAt() < b.createdAt())
                comparison = -1;
            else if (a.createdAt() > b.createdAt())
                comparison = 1;
        }
        
        // Apply sort direction
        // Invalid items stay at end (handled above with positive comparison)
        if (m_sortAscending)
            return comparison > 0;
        else
            return comparison < 0;
    });
}

bool TaskModel::createTask(const QString& title, const QString& description, int state)
{
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
    }

    QVariantList data;
    data << userId  // Valid UserId from Users table
         << title
         << description
         << state
         << QDateTime::currentDateTime()
         << QVariant()
         << QVariant();  // DeletedAt (null for new tasks)
    
    auto [result, taskId] = DBProcessing::instance().requestAddRow(DBTypes::DBTables::Tasks, data);
    
    if (result == DBTypes::DBResult::OK) {
        qDebug() << "Task created with ID:" << taskId;
        updateTask();
        return true;
    }
    
    qWarning() << "Failed to create task";
    return false;
}

bool TaskModel::updateTask(int taskId, const QString& title, const QString& description, int state)
{
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
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

    auto result = DBProcessing::instance().requestUpdate(DBTypes::DBTables::Tasks, columns, values);

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
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
    }
    
    // Soft delete: set DeletedAt timestamp
    QVector<QString> columns;
    columns << "Id" << "DeletedAt";
    
    QVariantList values;
    values << taskId
           << QDateTime::currentDateTime();
    
    auto result = DBProcessing::instance().requestUpdate(DBTypes::DBTables::Tasks, columns, values);
    
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
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
    }

    auto result = DBProcessing::instance().requestDelete(DBTypes::DBTables::Tasks,taskId);

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
    int userId = getCurrentUserId();
    if (userId < 0) {
        setError("No user signed in");
        return false;
    }

    // Remove DeletedAt (set NULL) and reset state to Active (0)
    QVector<QString> columns;
    columns << "Id" << "DeletedAt";

    QVariantList values;
    values << taskId
           << QVariant()  ;     // State -> Active

    auto result = DBProcessing::instance().requestUpdate(DBTypes::DBTables::Tasks, columns, values);

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

int TaskModel::filterMonth() const
{
    return m_filterMonth;
}

int TaskModel::filterYear() const
{
    return m_filterYear;
}

void TaskModel::setFilterMonth(int month, int year)
{
    if (m_filterMonth == month && m_filterYear == year)
        return;
    
    m_filterMonth = month;
    m_filterYear = year;
    emit filterMonthChanged();
    emit filterYearChanged();
    
    updateTask();
}

void TaskModel::onUserChanged()
{
    // When user changes in UserModel, reload tasks
    updateTask();
}
