#include "include/task.h"

Task::Task(QString title,
           QString description,
           DBTypes::TaskState taskState,
           QDateTime createdAt,
           DBTypes::DBIndex id)
    : m_id {id},
    m_title {std::move(title)},
    m_description {std::move(description)},
    m_taskState {static_cast<int>(taskState)},
    m_createdAt {std::move(createdAt)}
{
}

DBTypes::DBIndex Task::id() const
{
    return m_id;
}

QString Task::title() const
{
    return m_title;
}

QString Task::description() const
{
    return m_description;
}

DBTypes::TaskState Task::taskState() const
{
    return static_cast<DBTypes::TaskState>(m_taskState);
}

QDateTime Task::createdAt() const
{
    return m_createdAt;
}

QDateTime Task::updatedAt() const
{
    return m_updatedAt;
}

QDateTime Task::deletedAt() const
{
    return m_deletedAt;
}

void Task::setUpdatedAt(const QDateTime& dt)
{
    m_updatedAt = dt;
}

void Task::setDeletedAt(const QDateTime& dt)
{
    m_deletedAt = dt;
}

bool Task::isDeleted() const
{
    return m_deletedAt.isValid();
}
