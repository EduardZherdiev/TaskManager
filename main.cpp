#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include "include/taskmodel.h"
#include "include/Feedbackmodel.h"
#include "include/usermodel.h"
#include "include/openglbarchart.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QGuiApplication app(argc, argv);
    
    // Set application info for QSettings
    QCoreApplication::setOrganizationName("TaskManager");
    QCoreApplication::setOrganizationDomain("taskmanager.local");
    QCoreApplication::setApplicationName("TaskManager");
    
    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;
    
    // Create UserModel first
    UserModel* userModel = new UserModel();
    
    // Create TaskModel and set its UserModel reference
    TaskModel* taskModel = new TaskModel();
    taskModel->setUserModel(userModel);
    
    // Create FeedbackModel and set its UserModel reference
    FeedbackModel* feedbackModel = new FeedbackModel();
    feedbackModel->setUserModel(userModel);
    
    // Register TaskModel as singleton
    qmlRegisterSingletonInstance<TaskModel>("core", 1, 0, "TaskModel", taskModel);
    
    // Register UserModel as singleton
    qmlRegisterSingletonInstance<UserModel>("core", 1, 0, "UserModel", userModel);
    
    // Register FeedbackModel as singleton
    qmlRegisterSingletonInstance<FeedbackModel>("core", 1, 0, "FeedbackModel", feedbackModel);

    // Register OpenGL-based bar chart for QML usage
    qmlRegisterType<OpenGLBarChart>("core", 1, 0, "OpenGLBarChart");
    
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
