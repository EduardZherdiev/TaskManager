QT       += core gui quick quickcontrols2 sql opengl

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        database/dbconnectionmanager.cpp \
        database/dbmanager.cpp \
        database/dbmanipulator.cpp \
        database/dbprocessing.cpp \
        database/dbselector.cpp \
        src/feedbackmodel.cpp \
        src/feedbackreader.cpp \
        src/taskmodel.cpp \
        src/taskreader.cpp \
        src/usermodel.cpp \
        src/userreader.cpp \
        main.cpp \
        src/feedback.cpp \
        src/task.cpp \
        src/user.cpp \
        src/openglbarchart.cpp

RESOURCES += qml.qrc

HEADERS += \
    database/dbconnectionmanager.h \
    database/dbmanipulator.h \
    database/dbprocessing.h \
    database/dbselector.h \
    database/dbtypes.h \
    database/dbmanager.h \
    include/feedback.h \
    include/feedbackmodel.h \
    include/feedbackreader.h \
    include/task.h \
    include/taskmodel.h \
    include/taskreader.h \
    include/user.h \
    include/usermodel.h \
    include/userreader.h \
    include/openglbarchart.h

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD/qml
QML2_IMPORT_PATH += $$PWD/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
