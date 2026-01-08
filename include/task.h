#pragma once
#include <QString>
#include <QVariant>
#include "database/dbtypes.h"
#include <QDateTime>

class Task {
public:
    Task() = default;
    Task(QString title,QString description,DBTypes::TaskState taskState,QDateTime createdAt,DBTypes::DBIndex id);
    DBTypes::DBIndex id() const;

    QString title() const;
    QString description() const;
    DBTypes::TaskState taskState() const;

    bool isDeleted() const;
    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    QDateTime deletedAt() const;

    void setUpdatedAt(const QDateTime& dt);
    void setDeletedAt(const QDateTime& dt);

private:
    DBTypes::DBIndex m_id;
    QString m_title;
    QString m_description;
    int m_taskState;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QDateTime m_deletedAt;
};

