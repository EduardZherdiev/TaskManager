QT       += core gui quick quickcontrols2 sql opengl network

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
        src/openglbarchart.cpp \
        src/openglbarchart3d.cpp \
        src/keyboardhandler.cpp \
        argon2/src/argon2.c \
        argon2/src/core.c \
        argon2/src/encoding.c \
        argon2/src/thread.c \
        argon2/src/blake2/blake2b.c \
        argon2/src/ref.c \
        network/networkclient.cpp \
        network/syncmanager.cpp \
        src/userregistrationhandler.cpp

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
    include/openglbarchart.h \
    include/openglbarchart3d.h \
    include/keyboardhandler.h \
    network/networkclient.h \
    network/syncmanager.h \
    include/userregistrationhandler.h

# Argon2 include paths
INCLUDEPATH += $$PWD/argon2/include \
               $$PWD/argon2/src

win32:RC_FILE = TaskManager.rc

# Copy assets folder to build directory
win32 {
    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/debug
    } else {
        DESTDIR = $$OUT_PWD/release
    }
    
    copydata.commands = $(COPY_DIR) $$shell_path($$PWD/assets/img) $$shell_path($$DESTDIR/assets/img)
    first.depends = $(OBJECTS_DIR) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}
QML_IMPORT_PATH += $$PWD/qml
QML2_IMPORT_PATH += $$PWD/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
