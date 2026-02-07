#include "syncmanager.h"
#include "../database/dbprocessing.h"
#include <QSettings>
#include <QDebug>


SyncManager::SyncManager(QObject *parent)
    : QObject(parent), m_hasRemoteChanges(false)
{
    QSettings settings;
    m_lastSyncTime = settings.value("sync/lastSyncTime", QDateTime()).toDateTime();
}


void SyncManager::uploadToServer()
{
    emit syncStarted();
    qDebug() << "═══ UPLOAD START ═══";

    try {
        // Получить все локальные задачи
        auto [result, tasks] = DBProcessing::instance().requestTableData(DBTypes::DBTables::Tasks);
        
        if (result != DBTypes::DBResult::OK) {
            emit syncFailed("Failed to read local tasks");
            return;
        }

        qDebug() << "Local tasks count:" << tasks.size();
        emit uploadProgress(0, tasks.size());

        // Загрузить каждую задачу на сервер
        int uploaded = 0;
        for (const auto &taskRow : tasks) {
            // taskRow: [rowid, Id, UserId, Title, Description, State, CreatedAt, UpdatedAt, DeletedAt]
            if (taskRow.size() >= 7) {
                int taskId = taskRow[1].toInt();
                int userId = taskRow[2].toInt();
                QString title = taskRow[3].toString();
                QString description = taskRow[4].toString();
                int state = taskRow[5].toInt();

                qDebug() << "Uploading task:" << taskId << title;
            }
            uploaded++;
            emit uploadProgress(uploaded, tasks.size());
        }

        // Получить все локальные отзывы
        auto [fbResult, feedbacks] = DBProcessing::instance().requestTableData(DBTypes::DBTables::Feedbacks);
        
        if (fbResult == DBTypes::DBResult::OK) {
            qDebug() << "Local feedbacks count:" << feedbacks.size();
            
            for (const auto &fbRow : feedbacks) {
                // fbRow: [rowid, Id, UserId, Rate, Description, CreatedAt, DeletedAt]
                if (fbRow.size() >= 5) {
                    int feedbackId = fbRow[1].toInt();
                    int userId = fbRow[2].toInt();
                    int rate = fbRow[3].toInt();
                    QString description = fbRow[4].toString();

                    qDebug() << "Uploading feedback:" << feedbackId;
                    // TODO: Отправить на сервер через NetworkClient
                }
            }
        }

        saveLastSyncTime();
        emit syncCompleted("All changes uploaded to server!");
        qDebug() << "═══ UPLOAD COMPLETE ═══";

    } catch (const std::exception &e) {
        emit syncFailed(QString::fromStdString(std::string(e.what())));
        qDebug() << "Upload error:" << e.what();
    }
}


void SyncManager::downloadFromServer()
{
    emit syncStarted();
    qDebug() << "═══ DOWNLOAD START ═══";

    try {
        // TODO: Получить задачи с сервера
        // TODO: Получить отзывы с сервера
        // TODO: Сравнить с локальными данными
        // TODO: Обнаружить конфликты

        // Если есть конфликты
        if (!m_conflicts.isEmpty()) {
            qDebug() << "Conflicts detected:" << m_conflicts.size();
            emit requiresUserDecision(m_conflicts);
            return;
        }

        // Обновить локальную БД
        // TODO: Синхронизировать локальные данные

        m_hasRemoteChanges = false;
        saveLastSyncTime();
        emit syncCompleted("All changes downloaded from server!");
        qDebug() << "═══ DOWNLOAD COMPLETE ═══";

    } catch (const std::exception &e) {
        emit syncFailed(QString::fromStdString(std::string(e.what())));
        qDebug() << "Download error:" << e.what();
    }
}


void SyncManager::resolveConflict(int conflictId, bool useLocal)
{
    if (conflictId < 0 || conflictId >= m_conflicts.size()) {
        return;
    }

    const auto &conflict = m_conflicts[conflictId];
    qDebug() << "Resolving conflict:" << conflict.itemType << conflict.id 
             << "Using:" << (useLocal ? "LOCAL" : "REMOTE");

    if (useLocal) {
        // TODO: Отправить локальное значение на сервер (перезаписать)
    } else {
        // TODO: Обновить локальную БД с серверным значением
    }

    m_conflicts.removeAt(conflictId);

    if (m_conflicts.isEmpty()) {
        emit conflictsResolved();
    }
}


void SyncManager::detectLocalChanges()
{
    // Находим задачи/отзывы, изменённые после последней синхронизации
    qDebug() << "Detecting local changes since:" << m_lastSyncTime.toString();
    // TODO: Реализовать
}


void SyncManager::detectRemoteChanges()
{
    // Сравниваем локальные данные с сервером
    qDebug() << "Detecting remote changes";
    // TODO: Реализовать
}


void SyncManager::saveLastSyncTime()
{
    m_lastSyncTime = QDateTime::currentDateTime();
    QSettings settings;
    settings.setValue("sync/lastSyncTime", m_lastSyncTime);
    qDebug() << "Last sync time saved:" << m_lastSyncTime;
}
