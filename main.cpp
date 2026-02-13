#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include <QQmlContext>
#include "include/taskmodel.h"
#include "include/Feedbackmodel.h"
#include "include/usermodel.h"
#include "include/userregistrationhandler.h"
#include "include/userupdatehandler.h"
#include "include/openglbarchart.h"
#include "include/openglbarchart3d.h"
#include "network/networkclient.h"

class LanguageManager : public QObject
{
    Q_OBJECT

signals:
    void languageChanged();

public:
    LanguageManager(QGuiApplication &app, QQmlApplicationEngine &engine, QObject *parent = nullptr)
        : QObject(parent),
          m_app(app),
          m_engine(engine)
    {
    }

    Q_INVOKABLE void setLanguage(int language)
    {
        const QString qmFile = (language == 1) ? QStringLiteral(":/qml_ua.qm")
                                               : QStringLiteral(":/qml_en.qm");
        const QLocale locale = (language == 1) ? QLocale("uk_UA") : QLocale("en_US");

        m_app.removeTranslator(&m_translator);
        if (m_translator.load(qmFile)) {
            m_app.installTranslator(&m_translator);
        }

        QLocale::setDefault(locale);
        m_engine.retranslate();
        
        // Emit signal after retranslate is complete
        emit languageChanged();
    }

private:
    QGuiApplication &m_app;
    QQmlApplicationEngine &m_engine;
    QTranslator m_translator;
};

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QGuiApplication app(argc, argv);

    QSettings settings;
    settings.beginGroup("Application");
    const int savedLanguage = settings.value("language", 0).toInt();
    settings.endGroup();
    
    // Set application info for QSettings
    QCoreApplication::setOrganizationName("TaskManager");
    QCoreApplication::setOrganizationDomain("taskmanager.local");
    QCoreApplication::setApplicationName("TaskManager");
    
    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;

    LanguageManager languageManager(app, engine);
    languageManager.setLanguage(savedLanguage);
    engine.rootContext()->setContextProperty("LanguageManager", &languageManager);
    
    // Create UserModel first
    UserModel* userModel = new UserModel();
    
    // Create TaskModel and set its UserModel reference
    TaskModel* taskModel = new TaskModel();
    taskModel->setUserModel(userModel);
    
    // Create FeedbackModel and set its UserModel reference
    FeedbackModel* feedbackModel = new FeedbackModel();
    feedbackModel->setUserModel(userModel);
    
    // Create NetworkClient
    NetworkClient* networkClient = new NetworkClient();
    networkClient->setBaseUrl("https://taskmanagerserver-5j4u.onrender.com");
    
    // Set NetworkClient in UserModel
    userModel->setNetworkClient(networkClient);

    QObject::connect(networkClient, &NetworkClient::tasksReceived,
                     taskModel, &TaskModel::applyRemoteTasks);

    QObject::connect(networkClient, &NetworkClient::feedbacksReceived,
                     feedbackModel, &FeedbackModel::applyRemoteFeedbacks);
    
    // Create UserRegistrationHandler for server-based registration
    UserRegistrationHandler* registrationHandler = new UserRegistrationHandler(networkClient);

    // Create UserUpdateHandler for server-based profile updates
    UserUpdateHandler* updateHandler = new UserUpdateHandler(networkClient);
    
    // Set registration handler in UserModel
    userModel->setRegistrationHandler(registrationHandler);

    // Set update handler in UserModel
    userModel->setUpdateHandler(updateHandler);
    
    // Register TaskModel as singleton
    qmlRegisterSingletonInstance<TaskModel>("core", 1, 0, "TaskModel", taskModel);
    
    // Register UserModel as singleton
    qmlRegisterSingletonInstance<UserModel>("core", 1, 0, "UserModel", userModel);
    
    // Register FeedbackModel as singleton
    qmlRegisterSingletonInstance<FeedbackModel>("core", 1, 0, "FeedbackModel", feedbackModel);
    
    // Register NetworkClient as singleton
    qmlRegisterSingletonInstance<NetworkClient>("core", 1, 0, "NetworkClient", networkClient);
    
    // Register UserRegistrationHandler as singleton
    qmlRegisterSingletonInstance<UserRegistrationHandler>("core", 1, 0, "UserRegistrationHandler", registrationHandler);

    // Register OpenGL-based bar chart for QML usage
    qmlRegisterType<OpenGLBarChart>("core", 1, 0, "OpenGLBarChart");
    qmlRegisterType<OpenGLBarChart3D>("core", 1, 0, "OpenGLBarChart3D");
    
    engine.addImportPath(":/qml");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

#include "main.moc"
