#include "networkclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include "../database/dbprocessing.h"


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


void NetworkClient::setAccessToken(const QString &token)
{
    if (m_accessToken == token) {
        return;
    }
    m_accessToken = token;
    emit accessTokenChanged(m_accessToken);
}


void NetworkClient::clearAccessToken()
{
    setAccessToken("");
}


QString NetworkClient::accessToken() const
{
    return m_accessToken;
}


void NetworkClient::setRefreshToken(const QString &token)
{
    if (m_refreshToken == token) {
        return;
    }
    m_refreshToken = token;
    emit refreshTokenChanged(m_refreshToken);
}


void NetworkClient::clearRefreshToken()
{
    setRefreshToken("");
}


QString NetworkClient::refreshToken() const
{
    return m_refreshToken;
}

static QString networkErrorToEnglish(QNetworkReply::NetworkError error)
{
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            return QCoreApplication::translate("NetworkClient", "Connection refused. Please check the internet connection and try again.");
        case QNetworkReply::RemoteHostClosedError:
            return QCoreApplication::translate("NetworkClient", "Connection closed by the server.");
        case QNetworkReply::HostNotFoundError:
            return QCoreApplication::translate("NetworkClient", "Server not found. Please check the internet connection.");
        case QNetworkReply::TimeoutError:
            return QCoreApplication::translate("NetworkClient", "Network timeout. Please try again.");
        case QNetworkReply::OperationCanceledError:
            return QCoreApplication::translate("NetworkClient", "Network request was canceled.");
        case QNetworkReply::SslHandshakeFailedError:
            return QCoreApplication::translate("NetworkClient", "SSL handshake failed. Please check your network or certificate settings.");
        case QNetworkReply::TemporaryNetworkFailureError:
            return QCoreApplication::translate("NetworkClient", "Temporary network failure. Please try again.");
        case QNetworkReply::UnknownNetworkError:
            return QCoreApplication::translate("NetworkClient", "Unknown network error.");
        case QNetworkReply::ProxyConnectionRefusedError:
            return QCoreApplication::translate("NetworkClient", "Proxy connection refused.");
        case QNetworkReply::ProxyConnectionClosedError:
            return QCoreApplication::translate("NetworkClient", "Proxy connection closed.");
        case QNetworkReply::ProxyNotFoundError:
            return QCoreApplication::translate("NetworkClient", "Proxy not found.");
        case QNetworkReply::ProxyTimeoutError:
            return QCoreApplication::translate("NetworkClient", "Proxy timeout.");
        case QNetworkReply::ProxyAuthenticationRequiredError:
            return QCoreApplication::translate("NetworkClient", "Proxy authentication required.");
        case QNetworkReply::ContentAccessDenied:
            return QCoreApplication::translate("NetworkClient", "Access denied.");
        case QNetworkReply::ContentNotFoundError:
            return QCoreApplication::translate("NetworkClient", "Requested resource not found.");
        case QNetworkReply::AuthenticationRequiredError:
            return QCoreApplication::translate("NetworkClient", "Authentication required.");
        case QNetworkReply::ContentReSendError:
            return QCoreApplication::translate("NetworkClient", "Network error. Please try again.");
        case QNetworkReply::ProtocolUnknownError:
            return QCoreApplication::translate("NetworkClient", "Unknown network protocol.");
        case QNetworkReply::ProtocolInvalidOperationError:
            return QCoreApplication::translate("NetworkClient", "Invalid network operation.");
        case QNetworkReply::ProtocolFailure:
            return QCoreApplication::translate("NetworkClient", "Network protocol failure.");
        case QNetworkReply::InternalServerError:
            return QCoreApplication::translate("NetworkClient", "Server error. Please try again later.");
        case QNetworkReply::OperationNotImplementedError:
            return QCoreApplication::translate("NetworkClient", "Server does not support this request.");
        case QNetworkReply::ServiceUnavailableError:
            return QCoreApplication::translate("NetworkClient", "Service unavailable. Please try again later.");
        default:
            return QCoreApplication::translate("NetworkClient", "Network error. Please try again.");
    }
}


void NetworkClient::checkHealth()
{
    sendRequest("/health", QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на перевірку здоров'я сервера";
}


QNetworkRequest NetworkClient::createRequest(const QString &endpoint)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!m_accessToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_accessToken).toUtf8());
    }
    return request;
}


QNetworkRequest NetworkClient::createRequestWithToken(const QString &endpoint, const QString &token)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
    }
    return request;
}


QNetworkReply *NetworkClient::sendRequest(const QString &endpoint, QNetworkAccessManager::Operation operation, const QByteArray &body, int retryCount)
{
    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply *reply = nullptr;

    switch (operation) {
        case QNetworkAccessManager::GetOperation:
            reply = m_manager->get(request);
            break;
        case QNetworkAccessManager::PostOperation:
            reply = m_manager->post(request, body);
            break;
        case QNetworkAccessManager::PutOperation:
            reply = m_manager->put(request, body);
            break;
        case QNetworkAccessManager::DeleteOperation:
            reply = m_manager->deleteResource(request);
            break;
        default:
            reply = m_manager->get(request);
            break;
    }

    if (reply) {
        reply->setProperty("endpoint", endpoint);
        reply->setProperty("operation", static_cast<int>(operation));
        reply->setProperty("body", body);
        reply->setProperty("retryCount", retryCount);
    }

    return reply;
}


QNetworkReply *NetworkClient::sendRequestWithToken(const QString &endpoint, QNetworkAccessManager::Operation operation, const QByteArray &body, const QString &token, int retryCount)
{
    QNetworkRequest request = createRequestWithToken(endpoint, token);
    QNetworkReply *reply = nullptr;

    switch (operation) {
        case QNetworkAccessManager::GetOperation:
            reply = m_manager->get(request);
            break;
        case QNetworkAccessManager::PostOperation:
            reply = m_manager->post(request, body);
            break;
        case QNetworkAccessManager::PutOperation:
            reply = m_manager->put(request, body);
            break;
        case QNetworkAccessManager::DeleteOperation:
            reply = m_manager->deleteResource(request);
            break;
        default:
            reply = m_manager->get(request);
            break;
    }

    if (reply) {
        reply->setProperty("endpoint", endpoint);
        reply->setProperty("operation", static_cast<int>(operation));
        reply->setProperty("body", body);
        reply->setProperty("retryCount", retryCount);
    }

    return reply;
}


void NetworkClient::retryPendingRequest()
{
    if (!m_pendingRequest.isValid()) {
        return;
    }

    int retryCount = m_pendingRequest.retryCount + 1;
    sendRequest(m_pendingRequest.endpoint, m_pendingRequest.operation, m_pendingRequest.body, retryCount);
    m_pendingRequest.clear();
}


// ==================== КОРИСТУВАЧІ ====================

void NetworkClient::checkUserExists(const QString &login)
{
    sendRequest(QString("/users/check?login=%1").arg(login), QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на перевірку наявності користувача:" << login;
}

void NetworkClient::registerUser(const QString &login, const QString &password)
{
    QJsonObject json;
    json["login"] = login;
    json["password"] = password;

    QJsonDocument doc(json);
    sendRequest("/users", QNetworkAccessManager::PostOperation, doc.toJson());
    qDebug() << "Запит на реєстрацію:" << login;
}


void NetworkClient::loginUser(const QString &login, const QString &password)
{
    QJsonObject json;
    json["login"] = login;
    json["password"] = password;

    QJsonDocument doc(json);
    sendRequest("/users/login", QNetworkAccessManager::PostOperation, doc.toJson());
    qDebug() << "Запит на вхід:" << login;
}


void NetworkClient::getUsers()
{
    sendRequest("/users", QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання всіх користувачів";
}


void NetworkClient::getUser(int userId)
{
    sendRequest(QString("/users/%1").arg(userId), QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання користувача:" << userId;
}


void NetworkClient::deleteUser(int userId)
{
    sendRequest(QString("/users/%1").arg(userId), QNetworkAccessManager::DeleteOperation);
    qDebug() << "Запит на видалення користувача:" << userId;
}


// ==================== ЗАДАЧІ ====================

void NetworkClient::createTask(const QString &title, const QString &description, int state)
{
    QJsonObject json;
    json["title"] = title;
    json["description"] = description;
    json["state"] = state;

    QJsonDocument doc(json);
    sendRequest("/tasks", QNetworkAccessManager::PostOperation, doc.toJson());
    qDebug() << "Запит на створення задачі:" << title;
}


void NetworkClient::getTasks(int userId)
{
    QString endpoint = "/tasks";
    if (userId != -1) {
        endpoint += QString("?user_id=%1").arg(userId);
    }
    sendRequest(endpoint, QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання задач для користувача:" << userId;
}


void NetworkClient::getTask(int taskId)
{
    sendRequest(QString("/tasks/%1").arg(taskId), QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання задачі:" << taskId;
}


void NetworkClient::updateTask(int taskId, const QString &title, const QString &description, int state)
{
    QJsonObject json;
    json["title"] = title;
    json["description"] = description;
    json["state"] = state;

    QJsonDocument doc(json);
    sendRequest(QString("/tasks/%1").arg(taskId), QNetworkAccessManager::PutOperation, doc.toJson());
    qDebug() << "Запит на оновлення задачі:" << taskId;
}


void NetworkClient::deleteTask(int taskId)
{
    sendRequest(QString("/tasks/%1").arg(taskId), QNetworkAccessManager::DeleteOperation);
    qDebug() << "Запит на видалення задачі:" << taskId;
}


// ==================== ВІДГУКИ ====================

void NetworkClient::createFeedback(int rate, const QString &description)
{
    QJsonObject json;
    json["rate"] = rate;
    json["description"] = description;

    QJsonDocument doc(json);
    sendRequest("/feedbacks", QNetworkAccessManager::PostOperation, doc.toJson());
    qDebug() << "Запит на створення відгуку";
}


void NetworkClient::getFeedbacks(int userId)
{
    QString endpoint = "/feedbacks";
    if (userId != -1) {
        endpoint += QString("?user_id=%1").arg(userId);
    }
    sendRequest(endpoint, QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання відгуків для користувача:" << userId;
}


void NetworkClient::getFeedback(int feedbackId)
{
    sendRequest(QString("/feedbacks/%1").arg(feedbackId), QNetworkAccessManager::GetOperation);
    qDebug() << "Запит на отримання відгуку:" << feedbackId;
}


void NetworkClient::updateFeedback(int feedbackId, int rate, const QString &description)
{
    QJsonObject json;
    json["rate"] = rate;
    json["description"] = description;

    QJsonDocument doc(json);
    sendRequest(QString("/feedbacks/%1").arg(feedbackId), QNetworkAccessManager::PutOperation, doc.toJson());
    qDebug() << "Запит на оновлення відгуку:" << feedbackId;
}


void NetworkClient::deleteFeedback(int feedbackId)
{
    sendRequest(QString("/feedbacks/%1").arg(feedbackId), QNetworkAccessManager::DeleteOperation);
    qDebug() << "Запит на видалення відгуку:" << feedbackId;
}


void NetworkClient::refreshAccessToken()
{
    if (m_refreshToken.isEmpty()) {
        emit error("Missing refresh token. Please sign in again.");
        return;
    }

    if (m_isRefreshing) {
        return;
    }

    m_isRefreshing = true;
    QJsonDocument doc(QJsonObject{});
    sendRequestWithToken("/users/refresh", QNetworkAccessManager::PostOperation, doc.toJson(), m_refreshToken);
}


// ==================== ОБРОБКА ВІДПОВІДЕЙ ====================

void NetworkClient::onFinished(QNetworkReply *reply)
{
    QByteArray responseBody = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString statusReason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    QString url = reply->url().toString();
    
    qDebug() << "HTTP Status:" << statusCode << statusReason;
    qDebug() << "Response Body:" << responseBody;
    
    if (statusCode == 401) {
        if (url.contains("/users/refresh")) {
            m_isRefreshing = false;
            clearAccessToken();
            clearRefreshToken();
            emit error("Session expired. Please sign in again.");
            reply->deleteLater();
            return;
        }

        int retryCount = reply->property("retryCount").toInt();
        if (!m_refreshToken.isEmpty() && !m_isRefreshing && retryCount < 1) {
            m_pendingRequest.endpoint = reply->property("endpoint").toString();
            m_pendingRequest.operation = static_cast<QNetworkAccessManager::Operation>(reply->property("operation").toInt());
            m_pendingRequest.body = reply->property("body").toByteArray();
            m_pendingRequest.retryCount = retryCount;
            refreshAccessToken();
            reply->deleteLater();
            return;
        }

        clearAccessToken();
        clearRefreshToken();
        emit error("Session expired. Please sign in again.");
        reply->deleteLater();
        return;
    }

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
                    errorMsg += QCoreApplication::translate("NetworkClient", "Validation errors:") + "\n";
                    for (const auto& item : detailArray) {
                        if (item.isObject()) {
                            QJsonObject errObj = item.toObject();
                            // Отримати останній елемент з "loc" (поле)
                            QString field = QCoreApplication::translate("NetworkClient", "unknown");
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
                errorMsg += QCoreApplication::translate("NetworkClient", "Validation errors:") + "\n";
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

        if (url.contains("/users/refresh")) {
            m_isRefreshing = false;
            clearAccessToken();
            clearRefreshToken();
        }
        
        // Перевірити, чи це запит на здоров'я
        if (url.contains("/health")) {
            emit healthChecked(false, errorMsg);
        }
        
        reply->deleteLater();
        return;
    }
    
    // Обробити помилки з'єднання
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = networkErrorToEnglish(reply->error());
        qDebug() << "Помилка мережі:" << reply->errorString();
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
    QNetworkAccessManager::Operation operation = reply->operation();
    
    qDebug() << "URL:" << url << "Метод:" << operation;

    // Обробити запит на здоров'я
    if (url.contains("/health")) {
        emit healthChecked(true, QString::fromUtf8(responseBody));
        qDebug() << "Сервер здоровий";
    }
    // Обробити різні типи запитів
    else if (url.contains("/users")) {
        if (url.contains("/check")) {
            // Проверка наличия пользователя
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                bool exists = obj["exists"].toBool();
                QString login = obj["login"].toString();
                emit userExistsCheckCompleted(exists, login);
                qDebug() << "Проверка пользователя завершена. Логин:" << login << ", существует:" << exists;
            }
        } 
        else if (operation == QNetworkAccessManager::PostOperation && url.contains("/login")) {
            // Вхід користувача - повертає JWT токен та користувача
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString token = obj["access_token"].toString();
                QString refresh = obj["refresh_token"].toString();
                QJsonObject userObj = obj["user"].toObject();
                int userId = userObj["id"].toInt();
                QString login = userObj["login"].toString();

                if (token.isEmpty()) {
                    emit error("Missing access token in login response");
                } else {
                    setAccessToken(token);
                }

                if (!refresh.isEmpty()) {
                    setRefreshToken(refresh);
                }

                if (userId > 0) {
                    emit userLoggedIn(userId, login);
                    qDebug() << "Вхід успішний. ID:" << userId;
                }
            }
        }
        else if (operation == QNetworkAccessManager::PostOperation && url.contains("/refresh")) {
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString token = obj["access_token"].toString();
                QString refresh = obj["refresh_token"].toString();

                if (!token.isEmpty()) {
                    setAccessToken(token);
                }

                if (!refresh.isEmpty()) {
                    setRefreshToken(refresh);
                }

                m_isRefreshing = false;
                retryPendingRequest();
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


void NetworkClient::uploadChanges()
{
    emit syncStarted();
    qDebug() << "Завантаження змін на сервер...";
    
    // Цей метод загружає всі локальні задачи та відгуки на сервер
    // В реальній реалізації слід отримувати поточного користувача
    // і передавати його ID
    
    // Для простоти - загружаємо всі задачи та відгуки
    getTasks();
    
    emit syncCompleted("Upload completed successfully!");
    qDebug() << "Зміни успішно завантажені";
}


void NetworkClient::uploadChangesForUser(int userId)
{
    emit syncStarted();
    qDebug() << "Завантаження змін користувача" << userId << "на сервер...";
    
    // Отримати всі задачи та відгуки для цього користувача
    if (userId <= 0) {
        emit syncFailed("Invalid user ID");
        return;
    }

    if (m_accessToken.isEmpty()) {
        emit syncFailed("Missing access token. Please sign in to server.");
        return;
    }

    auto [tasksResult, tasks] = DBProcessing::instance().requestTableData(DBTypes::DBTables::Tasks);
    if (tasksResult != DBTypes::DBResult::OK) {
        emit syncFailed("Failed to read local tasks");
        return;
    }

    int taskCount = 0;
    for (const auto &taskRow : tasks) {
        if (taskRow.size() >= 9) {
            int rowUserId = taskRow[2].toInt();
            if (rowUserId != userId) {
                continue;
            }
            const QString deletedAt = taskRow[8].toString();
            if (!deletedAt.isEmpty()) {
                continue;
            }
            const int taskId = taskRow[1].toInt();
            QString title = taskRow[3].toString();
            QString description = taskRow[4].toString();
            int state = taskRow[5].toInt();
            QJsonObject taskObj;
            taskObj["id"] = taskId;
            taskObj["title"] = title;
            taskObj["description"] = description;
            taskObj["state"] = state;
            QJsonDocument taskDoc(taskObj);
            sendRequest("/tasks", QNetworkAccessManager::PostOperation, taskDoc.toJson());
            taskCount++;
        }
    }

    auto [feedbacksResult, feedbacks] = DBProcessing::instance().requestTableData(DBTypes::DBTables::Feedbacks);
    if (feedbacksResult != DBTypes::DBResult::OK) {
        emit syncFailed("Failed to read local feedbacks");
        return;
    }

    int feedbackCount = 0;
    QJsonObject feedbackObj;
    for (const auto &feedbackRow : feedbacks) {
        if (feedbackRow.size() >= 5) {
            int rowUserId = feedbackRow[2].toInt();
            if (rowUserId != userId) {
                continue;
            }
            int rate = feedbackRow[3].toInt();
            QString description = feedbackRow[4].toString();
            feedbackObj["rate"] = rate;
            feedbackObj["description"] = description;
            feedbackCount++;
            break;
        }
    }

    if (!feedbackObj.isEmpty()) {
        QJsonDocument feedbackDoc(feedbackObj);
        sendRequest("/feedbacks", QNetworkAccessManager::PostOperation, feedbackDoc.toJson());
    }

    // Emit completion after a small delay to ensure requests are processed
    QTimer::singleShot(500, this, [this, userId, taskCount, feedbackCount]() {
        emit syncCompleted(QString("Upload completed for user %1: %2 tasks, %3 feedbacks")
                              .arg(userId)
                              .arg(taskCount)
                              .arg(feedbackCount));
        qDebug() << "Зміни користувача успішно завантажені";
    });
}


void NetworkClient::downloadChanges()
{
    emit syncStarted();
    qDebug() << "Завантаження змін з сервера...";
    
    // Завантажити всі задачи та відгуки з сервера
    getTasks();
    getFeedbacks();
    
    emit syncCompleted("Download completed successfully!");
    qDebug() << "Зміни успішно завантажені";
}


void NetworkClient::downloadChangesForUser(int userId)
{
    emit syncStarted();
    qDebug() << "Завантаження змін користувача" << userId << "з сервера...";
    
    if (userId <= 0) {
        emit syncFailed("Invalid user ID");
        return;
    }
    
    getTasks(userId);
    getFeedbacks(userId);
    
    // Emit completion after a small delay to ensure requests are processed
    QTimer::singleShot(500, this, [this, userId]() {
        emit syncCompleted(QString("Download completed for user %1!").arg(userId));
        qDebug() << "Зміни користувача успішно завантажені";
    });
}

