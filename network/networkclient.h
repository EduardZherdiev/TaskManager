#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    // Встановити базовий URL сервера
    Q_INVOKABLE void setBaseUrl(const QString &url);
    
    // Перевірка здоров'я сервера
    Q_INVOKABLE void checkHealth();

    // Користувачі
    Q_INVOKABLE void registerUser(const QString &login, const QString &password);
    Q_INVOKABLE void loginUser(const QString &login, const QString &password);
    Q_INVOKABLE void getUsers();
    Q_INVOKABLE void getUser(int userId);
    Q_INVOKABLE void deleteUser(int userId);

    // Задачі
    Q_INVOKABLE void createTask(int userId, const QString &title, const QString &description, int state = 0);
    Q_INVOKABLE void getTasks(int userId = -1);
    Q_INVOKABLE void getTask(int taskId);
    Q_INVOKABLE void updateTask(int taskId, const QString &title, const QString &description, int state);
    Q_INVOKABLE void deleteTask(int taskId);

    // Відгуки
    Q_INVOKABLE void createFeedback(int userId, int rate, const QString &description);
    Q_INVOKABLE void getFeedbacks(int userId = -1);
    Q_INVOKABLE void getFeedback(int feedbackId);
    Q_INVOKABLE void updateFeedback(int feedbackId, int rate, const QString &description);
    Q_INVOKABLE void deleteFeedback(int feedbackId);

signals:
    // Здоров'я сервера
    void healthChecked(bool isHealthy, const QString &message);
    
    // Користувачі
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

    // Помилки
    void error(const QString &message);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString m_baseUrl;

    // Допоміжні методи
    QNetworkRequest createRequest(const QString &endpoint);
};
