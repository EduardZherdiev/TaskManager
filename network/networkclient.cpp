#include "networkclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    // Встановити базовий URL за замовчуванням (локально)
    m_baseUrl = "http://127.0.0.1:8000";

    // Підключити сигнал завершення запиту
    connect(m_manager, &QNetworkAccessManager::finished, this, &NetworkClient::onFinished);
}


NetworkClient::~NetworkClient()
{
}


void NetworkClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
    qDebug() << "Базовий URL встановлено:" << m_baseUrl;
}


void NetworkClient::checkHealth()
{
    QNetworkRequest request = createRequest("/health");
    m_manager->get(request);
    qDebug() << "Запит на перевірку здоров'я сервера";
}


QNetworkRequest NetworkClient::createRequest(const QString &endpoint)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}


// ==================== КОРИСТУВАЧІ ====================

void NetworkClient::registerUser(const QString &login, const QString &password)
{
    QNetworkRequest request = createRequest("/users");

    QJsonObject json;
    json["login"] = login;
    json["password"] = password;

    QJsonDocument doc(json);
    m_manager->post(request, doc.toJson());
    qDebug() << "Запит на реєстрацію:" << login;
}


void NetworkClient::loginUser(const QString &login, const QString &password)
{
    QNetworkRequest request = createRequest("/users/login");

    QJsonObject json;
    json["login"] = login;
    json["password"] = password;

    QJsonDocument doc(json);
    m_manager->post(request, doc.toJson());
    qDebug() << "Запит на вхід:" << login;
}


void NetworkClient::getUsers()
{
    QNetworkRequest request = createRequest("/users");
    m_manager->get(request);
    qDebug() << "Запит на отримання всіх користувачів";
}


void NetworkClient::getUser(int userId)
{
    QNetworkRequest request = createRequest(QString("/users/%1").arg(userId));
    m_manager->get(request);
    qDebug() << "Запит на отримання користувача:" << userId;
}


void NetworkClient::deleteUser(int userId)
{
    QNetworkRequest request = createRequest(QString("/users/%1").arg(userId));
    m_manager->deleteResource(request);
    qDebug() << "Запит на видалення користувача:" << userId;
}


// ==================== ЗАДАЧІ ====================

void NetworkClient::createTask(int userId, const QString &title, const QString &description, int state)
{
    QNetworkRequest request = createRequest(QString("/tasks?user_id=%1").arg(userId));

    QJsonObject json;
    json["title"] = title;
    json["description"] = description;
    json["state"] = state;

    QJsonDocument doc(json);
    m_manager->post(request, doc.toJson());
    qDebug() << "Запит на створення задачі:" << title;
}


void NetworkClient::getTasks(int userId)
{
    QString endpoint = "/tasks";
    if (userId != -1) {
        endpoint += QString("?user_id=%1").arg(userId);
    }
    QNetworkRequest request = createRequest(endpoint);
    m_manager->get(request);
    qDebug() << "Запит на отримання задач для користувача:" << userId;
}


void NetworkClient::getTask(int taskId)
{
    QNetworkRequest request = createRequest(QString("/tasks/%1").arg(taskId));
    m_manager->get(request);
    qDebug() << "Запит на отримання задачі:" << taskId;
}


void NetworkClient::updateTask(int taskId, const QString &title, const QString &description, int state)
{
    QNetworkRequest request = createRequest(QString("/tasks/%1").arg(taskId));

    QJsonObject json;
    json["title"] = title;
    json["description"] = description;
    json["state"] = state;

    QJsonDocument doc(json);
    m_manager->put(request, doc.toJson());
    qDebug() << "Запит на оновлення задачі:" << taskId;
}


void NetworkClient::deleteTask(int taskId)
{
    QNetworkRequest request = createRequest(QString("/tasks/%1").arg(taskId));
    m_manager->deleteResource(request);
    qDebug() << "Запит на видалення задачі:" << taskId;
}


// ==================== ВІДГУКИ ====================

void NetworkClient::createFeedback(int userId, int rate, const QString &description)
{
    QNetworkRequest request = createRequest(QString("/feedbacks?user_id=%1").arg(userId));

    QJsonObject json;
    json["rate"] = rate;
    json["description"] = description;

    QJsonDocument doc(json);
    m_manager->post(request, doc.toJson());
    qDebug() << "Запит на створення відгуку для користувача:" << userId;
}


void NetworkClient::getFeedbacks(int userId)
{
    QString endpoint = "/feedbacks";
    if (userId != -1) {
        endpoint += QString("?user_id=%1").arg(userId);
    }
    QNetworkRequest request = createRequest(endpoint);
    m_manager->get(request);
    qDebug() << "Запит на отримання відгуків для користувача:" << userId;
}


void NetworkClient::getFeedback(int feedbackId)
{
    QNetworkRequest request = createRequest(QString("/feedbacks/%1").arg(feedbackId));
    m_manager->get(request);
    qDebug() << "Запит на отримання відгуку:" << feedbackId;
}


void NetworkClient::updateFeedback(int feedbackId, int rate, const QString &description)
{
    QNetworkRequest request = createRequest(QString("/feedbacks/%1").arg(feedbackId));

    QJsonObject json;
    json["rate"] = rate;
    json["description"] = description;

    QJsonDocument doc(json);
    m_manager->put(request, doc.toJson());
    qDebug() << "Запит на оновлення відгуку:" << feedbackId;
}


void NetworkClient::deleteFeedback(int feedbackId)
{
    QNetworkRequest request = createRequest(QString("/feedbacks/%1").arg(feedbackId));
    m_manager->deleteResource(request);
    qDebug() << "Запит на видалення відгуку:" << feedbackId;
}


// ==================== ОБРОБКА ВІДПОВІДЕЙ ====================

void NetworkClient::onFinished(QNetworkReply *reply)
{
    QByteArray responseBody = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString statusReason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    
    qDebug() << "HTTP Status:" << statusCode << statusReason;
    qDebug() << "Response Body:" << responseBody;
    
    // Обробити помилки (4xx, 5xx статус коди)
    if (statusCode >= 400) {
        QString errorMsg = QString::number(statusCode) + " " + statusReason + "\n";
        
        // Спробуємо парсити JSON помилку від сервера
        QJsonDocument errorDoc = QJsonDocument::fromJson(responseBody);
        if (errorDoc.isObject()) {
            QJsonObject obj = errorDoc.object();
            if (obj.contains("detail")) {
                QJsonValue detail = obj["detail"];
                
                // Перевірити чи detail є масивом (FastAPI валидація) або строкою
                if (detail.isArray()) {
                    QJsonArray detailArray = detail.toArray();
                    errorMsg += "Validation errors:\n";
                    for (const auto& item : detailArray) {
                        if (item.isObject()) {
                            QJsonObject errObj = item.toObject();
                            // Отримати останній елемент з "loc" (поле)
                            QString field = "unknown";
                            if (errObj.contains("loc")) {
                                QJsonArray locArray = errObj["loc"].toArray();
                                if (locArray.count() > 0) {
                                    field = locArray.last().toString();
                                }
                            }
                            QString msg = errObj["msg"].toString();
                            errorMsg += "  " + field + ": " + msg + "\n";
                        }
                    }
                } else {
                    // Якщо це просто строка
                    errorMsg += detail.toString();
                }
            } else if (obj.contains("validation_errors")) {
                // Для списку помилок валидації
                errorMsg += "Validation errors:\n";
                QJsonArray errors = obj["validation_errors"].toArray();
                for (const auto& error : errors) {
                    if (error.isObject()) {
                        QJsonObject errObj = error.toObject();
                        QString field = errObj["loc"].toArray().last().toString();
                        QString msg = errObj["msg"].toString();
                        errorMsg += field + ": " + msg + "\n";
                    }
                }
            }
        } else if (statusCode >= 500) {
            // Для серверних помилок без JSON
            errorMsg += QString::fromUtf8(responseBody);
        }
        
        qDebug() << "Помилка:" << errorMsg;
        emit error(errorMsg);
        
        // Перевірити, чи це запит на здоров'я
        QString url = reply->url().toString();
        if (url.contains("/health")) {
            emit healthChecked(false, errorMsg);
        }
        
        reply->deleteLater();
        return;
    }
    
    // Обробити помилки з'єднання
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        qDebug() << "Помилка мережі:" << errorMsg;
        emit error(errorMsg);
        
        QString url = reply->url().toString();
        if (url.contains("/health")) {
            emit healthChecked(false, errorMsg);
        }
        
        reply->deleteLater();
        return;
    }

    qDebug() << "Відповідь від сервера:" << responseBody;

    // Парсити JSON
    QJsonDocument doc = QJsonDocument::fromJson(responseBody);
    
    // Визначити тип запиту за URL та методом
    QString url = reply->url().toString();
    QNetworkAccessManager::Operation operation = reply->operation();
    
    qDebug() << "URL:" << url << "Метод:" << operation;

    // Обробити запит на здоров'я
    if (url.contains("/health")) {
        emit healthChecked(true, QString::fromUtf8(responseBody));
        qDebug() << "Сервер здоровий";
    }
    // Обробити різні типи запитів
    else if (url.contains("/users")) {
        if (operation == QNetworkAccessManager::PostOperation && url.contains("/login")) {
            // Вхід користувача - повертає JSON об'єкт
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                int userId = obj["id"].toInt();
                QString login = obj["login"].toString();
                emit userLoggedIn(userId, login);
                qDebug() << "Вхід успішний. ID:" << userId;
            }
        } 
        else if (operation == QNetworkAccessManager::PostOperation) {
            // Реєстрація користувача - повертає ID
            bool ok = false;
            int userId = 0;
            
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                userId = obj["id"].toInt();
            } else {
                // Сервер може повертати просто число
                QString jsonStr = QString::fromUtf8(responseBody);
                userId = jsonStr.toInt(&ok);
            }
            
            if (userId > 0) {
                emit userRegistered(userId);
                qDebug() << "Реєстрація успішна. ID:" << userId;
            }
        } 
        else if (operation == QNetworkAccessManager::GetOperation) {
            // GET запит
            if (doc.isArray()) {
                // Отримання всіх користувачів
                emit usersReceived(doc.array());
                qDebug() << "Користувачів отримано:" << doc.array().count();
            } else if (doc.isObject()) {
                // Отримання одного користувача
                emit userReceived(doc.object());
                qDebug() << "Користувач отримано";
            }
        } 
        else if (operation == QNetworkAccessManager::DeleteOperation) {
            // Видалення користувача
            emit userDeleted();
            qDebug() << "Користувач видалений";
        }
    }
    else if (url.contains("/tasks")) {
        if (operation == QNetworkAccessManager::PostOperation) {
            // Створення задачі - повертає ID
            bool ok = false;
            int taskId = 0;
            
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                taskId = obj["id"].toInt();
            } else {
                QString jsonStr = QString::fromUtf8(responseBody);
                taskId = jsonStr.toInt(&ok);
            }
            
            if (taskId > 0) {
                emit taskCreated(taskId);
                qDebug() << "Задача створена. ID:" << taskId;
            }
        }
        else if (operation == QNetworkAccessManager::GetOperation) {
            if (doc.isArray()) {
                emit tasksReceived(doc.array());
                qDebug() << "Задач отримано:" << doc.array().count();
            } else if (doc.isObject()) {
                emit taskReceived(doc.object());
                qDebug() << "Задача отримана";
            }
        }
        else if (operation == QNetworkAccessManager::PutOperation) {
            emit taskUpdated();
            qDebug() << "Задача оновлена";
        }
        else if (operation == QNetworkAccessManager::DeleteOperation) {
            emit taskDeleted();
            qDebug() << "Задача видалена";
        }
    }
    else if (url.contains("/feedbacks")) {
        if (operation == QNetworkAccessManager::PostOperation) {
            // Створення відгуку - повертає ID
            bool ok = false;
            int feedbackId = 0;
            
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                feedbackId = obj["id"].toInt();
            } else {
                QString jsonStr = QString::fromUtf8(responseBody);
                feedbackId = jsonStr.toInt(&ok);
            }
            
            if (feedbackId > 0) {
                emit feedbackCreated(feedbackId);
                qDebug() << "Відгук створений. ID:" << feedbackId;
            }
        }
        else if (operation == QNetworkAccessManager::GetOperation) {
            if (doc.isArray()) {
                emit feedbacksReceived(doc.array());
                qDebug() << "Відгуків отримано:" << doc.array().count();
            } else if (doc.isObject()) {
                emit feedbackReceived(doc.object());
                qDebug() << "Відгук отримано";
            }
        }
        else if (operation == QNetworkAccessManager::PutOperation) {
            emit feedbackUpdated();
            qDebug() << "Відгук оновлений";
        }
        else if (operation == QNetworkAccessManager::DeleteOperation) {
            emit feedbackDeleted();
            qDebug() << "Відгук видалений";
        }
    }

    reply->deleteLater();
}
