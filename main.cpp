#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>
#include "include/taskmodel.h"
#include "include/callbackmodel.h"
#include "include/usermodel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    
    // Set application info for QSettings
    QCoreApplication::setOrganizationName("TaskManager");
    QCoreApplication::setOrganizationDomain("taskmanager.local");
    QCoreApplication::setApplicationName("TaskManager");
    
    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;
    // Register TaskModel as singleton so all QML files use the same instance
    qmlRegisterSingletonType<TaskModel>("core", 1, 0, "TaskModel", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new TaskModel();
    });
    
    // Register UserModel as singleton for authentication
    qmlRegisterSingletonType<UserModel>("core", 1, 0, "UserModel", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new UserModel();
    });
    
    qmlRegisterType<CallbackModel>("core", 1, 0, "CallbackModel");
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
