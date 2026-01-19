#pragma once

#include <QObject>
#include <QEvent>

class KeyboardHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardHandler(QObject* parent = nullptr);
    
    static void registerMe(const std::string& moduleName);
    
    // Invokable methods to check keyboard input based on physical key position (layout-independent)
    Q_INVOKABLE bool isKeyLeft(int key, const QString& text);
    Q_INVOKABLE bool isKeyRight(int key, const QString& text);
    Q_INVOKABLE bool isKeyUp(int key, const QString& text);
    Q_INVOKABLE bool isKeyDown(int key, const QString& text);
};

