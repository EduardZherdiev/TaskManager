#include "taskmodel.h"
#include <QQmlEngine>
#include <QDebug>
#include <QDateTime>

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
    std::tie(requestResult, TaskResult) = m_reader.requestTaskBrowse();

    qDebug() << "TaskModel::updateTask requestResult=" << requestResult << " rows=" << TaskResult.size();

    if (requestResult) {
        beginResetModel();
        m_Tasks.swap(TaskResult);
        endResetModel();

        // Debug: log loaded tasks
        for (const auto &t : m_Tasks) {
            qDebug() << "Loaded task:" << t.title() << " "<<t.createdAt()<<" "

                     << "createdAt:" << (t.createdAt().isValid() ? t.createdAt().toString(Qt::ISODate) : "invalid")
                     << "taskState:" << static_cast<int>(t.taskState());
        }
    } else {
        qWarning() << "TaskModel::updateTask failed to load tasks";
    }

    return requestResult;
}
