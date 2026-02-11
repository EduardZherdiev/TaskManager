#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>


class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    // Встановити базовий URL сервера
    Q_INVOKABLE void setBaseUrl(const QString &url);

    // JWT access token
    Q_INVOKABLE void setAccessToken(const QString &token);
    Q_INVOKABLE void clearAccessToken();
    Q_INVOKABLE QString accessToken() const;

    Q_INVOKABLE void setRefreshToken(const QString &token);
    Q_INVOKABLE void clearRefreshToken();
    Q_INVOKABLE QString refreshToken() const;
    Q_INVOKABLE void refreshAccessToken();
    
    // Перевірка здоров'я сервера
    Q_INVOKABLE void checkHealth();

    // Користувачі
    Q_INVOKABLE void checkUserExists(const QString &login);
    Q_INVOKABLE void registerUser(const QString &login, const QString &password);
    Q_INVOKABLE void loginUser(const QString &login, const QString &password);
    Q_INVOKABLE void getUsers();
    Q_INVOKABLE void getUser(int userId);
    Q_INVOKABLE void deleteUser(int userId);

    // Задачі
    Q_INVOKABLE void createTask(const QString &title, const QString &description, int state = 0);
    Q_INVOKABLE void getTasks(int userId = -1);
    Q_INVOKABLE void getTask(int taskId);
    Q_INVOKABLE void updateTask(int taskId, const QString &title, const QString &description, int state);
    Q_INVOKABLE void deleteTask(int taskId);

    // Відгуки
    Q_INVOKABLE void createFeedback(int rate, const QString &description);
    Q_INVOKABLE void getFeedbacks(int userId = -1);
    Q_INVOKABLE void getFeedback(int feedbackId);
    Q_INVOKABLE void updateFeedback(int feedbackId, int rate, const QString &description);
    Q_INVOKABLE void deleteFeedback(int feedbackId);

    // Синхронізація
    Q_INVOKABLE void uploadChanges();
    Q_INVOKABLE void uploadChangesForUser(int userId);
    Q_INVOKABLE void downloadChanges();
    Q_INVOKABLE void downloadChangesForUser(int userId);

signals:
    void accessTokenChanged(const QString &token);
    void refreshTokenChanged(const QString &token);
    // Здоров'я сервера
    void healthChecked(bool isHealthy, const QString &message);
    
    // Користувачі
    void userExistsCheckCompleted(bool exists, const QString &login);
    void userRegistered(int userId);
    void userLoggedIn(int userId, const QString &login);
    void usersReceived(const QJsonArray &users);
    void userReceived(const QJsonObject &user);
    void userDeleted();

    // Задачі
    void taskCreated(int taskId);
    void tasksReceived(const QJsonArray &tasks);
    void taskReceived(const QJsonObject &task);
    void taskUpdated();
    void taskDeleted();

    // Відгуки
    void feedbackCreated(int feedbackId);
    void feedbacksReceived(const QJsonArray &feedbacks);
    void feedbackReceived(const QJsonObject &feedback);
    void feedbackUpdated();
    void feedbackDeleted();

    // Синхронізація
    void syncStarted();
    void syncCompleted(const QString &message);
    void syncFailed(const QString &error);

    // Помилки
    void error(const QString &message);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    struct PendingRequest {
        QString endpoint;
        QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation;
        QByteArray body;
        int retryCount = 0;

        void clear() {
            endpoint.clear();
            body.clear();
            operation = QNetworkAccessManager::GetOperation;
            retryCount = 0;
        }

        bool isValid() const {
            return !endpoint.isEmpty();
        }
    };

    QNetworkAccessManager *m_manager;
    QString m_baseUrl;
    QString m_accessToken;
    QString m_refreshToken;
    bool m_isRefreshing = false;
    QList<PendingRequest> m_pendingRequests;

    // Допоміжні методи
    QNetworkRequest createRequest(const QString &endpoint);
    QNetworkRequest createRequestWithToken(const QString &endpoint, const QString &token);
    QNetworkReply *sendRequest(const QString &endpoint, QNetworkAccessManager::Operation operation, const QByteArray &body = QByteArray(), int retryCount = 0);
    QNetworkReply *sendRequestWithToken(const QString &endpoint, QNetworkAccessManager::Operation operation, const QByteArray &body, const QString &token, int retryCount = 0);
    void retryPendingRequest();
};
