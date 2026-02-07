#pragma once
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>


struct SyncConflict {
    QString itemType; // "task" или "feedback"
    int id;
    QString field;
    QVariant localValue;
    QVariant remoteValue;
    QDateTime localModified;
    QDateTime remoteModified;
};


class SyncManager : public QObject
{
    Q_OBJECT

public:
    explicit SyncManager(QObject *parent = nullptr);

    // Синхронизация
    void uploadToServer();
    void downloadFromServer();
    void resolveConflict(int conflictId, bool useLocal);

    // Получение информации
    QVector<SyncConflict> getConflicts() const { return m_conflicts; }
    QDateTime getLastSyncTime() const { return m_lastSyncTime; }
    bool hasRemoteChanges() const { return m_hasRemoteChanges; }

signals:
    // Синхронизация
    void syncStarted();
    void uploadProgress(int current, int total);
    void downloadProgress(int current, int total);
    void syncCompleted(const QString &message);
    void syncFailed(const QString &error);

    // Конфликты
    void conflictDetected(const SyncConflict &conflict);
    void conflictsResolved();

    // Уведомления
    void remoteChangesDetected(int taskCount, int feedbackCount);
    void requiresUserDecision(const QVector<SyncConflict> &conflicts);

private:
    void detectLocalChanges();
    void detectRemoteChanges();
    void resolveConflicts();
    void saveLastSyncTime();

    QVector<SyncConflict> m_conflicts;
    QDateTime m_lastSyncTime;
    bool m_hasRemoteChanges;
};
