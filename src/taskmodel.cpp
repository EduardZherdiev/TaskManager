#include "include/taskmodel.h"
#include "include/usermodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
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
    
    // Initialize filter with current month
    QDate today = QDate::currentDate();
    m_filterMonth = today.month() - 1;  // 0-11
    m_filterYear = today.year();
    qDebug() << "[INIT] TaskModel initialized with current month filter:" << today.toString("MMMM yyyy");
}

static QString normalizeIsoDateTime(const QString &value)
{
    if (value.isEmpty()) {
        return QString();
    }

    QDateTime dt = QDateTime::fromString(value, Qt::ISODateWithMs);
    if (!dt.isValid()) {
        dt = QDateTime::fromString(value, Qt::ISODate);
    }

    return dt.isValid() ? dt.toString(Qt::ISODate) : value;
}

void TaskModel::applyRemoteTasks(const QJsonArray &tasks)
{
    if (tasks.isEmpty()) {
        updateTask();
        return;
    }

    int currentUserId = getCurrentUserId();
    for (const auto &taskValue : tasks) {
        if (!taskValue.isObject()) {
            continue;
        }

        const QJsonObject obj = taskValue.toObject();
        const int taskId = obj.value("id").toInt();
        const int userId = obj.value("user_id").toInt();

        if (taskId <= 0) {
            continue;
        }

        if (currentUserId >= 0 && userId != currentUserId) {
            continue;
        }

        const QString title = obj.value("title").toString();
        const QString description = obj.value("description").toString();
        const int state = obj.value("state").toInt();

        const QString createdAt = normalizeIsoDateTime(obj.value("created_at").toString());
        const QString updatedAt = normalizeIsoDateTime(obj.value("updated_at").toString());
        const QString deletedAt = normalizeIsoDateTime(obj.value("deleted_at").toString());

        QVariantList args;
        args << taskId;
        auto existing = DBProcessing::instance().requestTableDataWhere(DBTypes::DBTables::Tasks,
                                                                       "Id = ?",
                                                                       args);

        if (existing.first == DBTypes::DBResult::OK && !existing.second.empty()) {
            const auto &row = existing.second.front();
            const int rowId = row[0].toInt();

            QVector<QString> columns;
            QVariantList values;
            columns << "Id";
            values << rowId;

            columns << "UserId" << "Title" << "Description" << "State" << "CreatedAt" << "UpdatedAt" << "DeletedAt";
            values << userId
                   << title
                   << description
                   << state
                   << createdAt
                   << updatedAt
                   << (deletedAt.isEmpty() ? QVariant() : QVariant(deletedAt));

            DBProcessing::instance().requestUpdate(DBTypes::DBTables::Tasks, columns, values);
        } else {
            QVariantList data;
            data << taskId
                 << userId
                 << title
                 << description
                 << state
                 << createdAt
                 << updatedAt
                 << (deletedAt.isEmpty() ? QVariant() : QVariant(deletedAt));

            DBProcessing::instance().requestAddRow(DBTypes::DBTables::Tasks, data);
        }
    }

    updateTask();
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
    // Pass month/year filter directly to database query
    std::tie(requestResult, TaskResult) = m_reader.requestTaskBrowse(m_showDeleted, userId, m_filterMonth, m_filterYear);
    qDebug() << "Tasks loaded, count:" << TaskResult.size();
    
    if (requestResult) {
        qDebug() << "Starting model update...";
        beginResetModel();
        m_Tasks.swap(TaskResult);
        qDebug() << "Starting sort...";
        sortTasks();
        qDebug() << "Sort completed";
        endResetModel();
        qDebug() << "Model update completed";
        
        // Update statistics
        updateStats();
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
    // Validate title
    if (title.trimmed().isEmpty()) {
        setError(tr("Task title cannot be empty"));
        return false;
    }

    // Validate state (0=Active, 1=Completed, 2=Archived)
    if (state < 0 || state > 2) {
        setError(tr("Invalid task state"));
        return false;
    }

    int userId = getCurrentUserId();
    if (userId < 0) {
        setError(tr("No user signed in"));
        return false;
    }

    QVariantList data;
        data << QVariant() // Id (NULL for local insert)
             << userId  // Valid UserId from Users table
         << title
         << description
         << state
         << QDateTime::currentDateTimeUtc()
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
    // Validate title
    if (title.trimmed().isEmpty()) {
        setError(tr("Task title cannot be empty"));
        return false;
    }

    // Validate state (0=Active, 1=Completed, 2=Archived)
    if (state < 0 || state > 2) {
        setError(tr("Invalid task state"));
        return false;
    }

    int userId = getCurrentUserId();
    if (userId < 0) {
        setError(tr("No user signed in"));
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
        setError(tr("No user signed in"));
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
        setError(tr("No user signed in"));
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
        setError(tr("No user signed in"));
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
    
    // Log filter parameters
    if (month < 0 || year < 0) {
        qDebug() << "[SQL FILTER] Showing all months (no date filter)";
    } else {
        qDebug() << "[SQL FILTER] Month:" << month << "Year:" << year;
        qDebug() << "[SQL FILTER] Filter will show tasks created in" << QDate(year, month + 1, 1).toString("MMMM yyyy");
    }
    
    emit filterMonthChanged();
    emit filterYearChanged();
    
    updateTask();
}

int TaskModel::archivedCount() const
{
    return m_archivedCount;
}

int TaskModel::inProgressCount() const
{
    return m_inProgressCount;
}

int TaskModel::completedCount() const
{
    return m_completedCount;
}

void TaskModel::updateStats()
{
    int userId = getCurrentUserId();
    if (userId < 0) {
        m_archivedCount = 0;
        m_inProgressCount = 0;
        m_completedCount = 0;
        emit statsChanged();
        return;
    }

    // Count tasks by status
    m_archivedCount = 0;
    m_inProgressCount = 0;
    m_completedCount = 0;
    
    qDebug() << "TaskModel::updateStats() - total tasks:" << m_Tasks.size();
    
    for (const auto& task : m_Tasks) {
        int state = static_cast<int>(task.taskState());
        switch (state) {
            case 0:
                m_inProgressCount++;
                break;
            case 1:
                m_completedCount++;
                break;
            case 2:
                m_archivedCount++;
                break;
        }
    }
    
    qDebug() << "Stats: notStarted:" << m_archivedCount << "inProgress:" << m_inProgressCount << "completed:" << m_completedCount;
    
    emit statsChanged();
}

void TaskModel::onUserChanged()
{
    // When user changes in UserModel, reload tasks
    updateTask();
}
