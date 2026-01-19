#include "include/keyboardhandler.h"
#include <QKeyEvent>
#include <QQmlEngine>

KeyboardHandler::KeyboardHandler(QObject* parent)
    : QObject(parent)
{
}

void KeyboardHandler::registerMe(const std::string& moduleName)
{
    qmlRegisterType<KeyboardHandler>(moduleName.c_str(), 1, 0, "KeyboardHandler");
}

// Check if key is on QWERTY-A / ЙЦУКЕН-Ф position (left rotation)
bool KeyboardHandler::isKeyLeft(int key, const QString& text)
{
    // Check by Qt key code (only works for English layout)
    if (key == Qt::Key_A)
        return true;
    
    // Check by text (works for any layout with corresponding character)
    QString lower = text.toLower();
    return lower == "a" || lower == "ф";  // English or Russian/Ukrainian
}

// Check if key is on QWERTY-D / ЙЦУКЕН-В position (right rotation)
bool KeyboardHandler::isKeyRight(int key, const QString& text)
{
    if (key == Qt::Key_D)
        return true;
    
    QString lower = text.toLower();
    return lower == "d" || lower == "в";  // English or Russian/Ukrainian
}

// Check if key is on QWERTY-W / ЙЦУКЕН-Ц position (up rotation)
bool KeyboardHandler::isKeyUp(int key, const QString& text)
{
    if (key == Qt::Key_W)
        return true;
    
    QString lower = text.toLower();
    return lower == "w" || lower == "ц";  // English or Russian/Ukrainian
}

// Check if key is on QWERTY-S / ЙЦУКЕН-Ы position (down rotation)
bool KeyboardHandler::isKeyDown(int key, const QString& text)
{
    if (key == Qt::Key_S)
        return true;
    
    QString lower = text.toLower();
    return lower == "s" || lower == "ы";  // English or Russian/Ukrainian
}
