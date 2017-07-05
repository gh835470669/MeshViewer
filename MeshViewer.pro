#-------------------------------------------------
#
# Project created by QtCreator 2017-05-18T21:35:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MeshViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/mainwindow.cpp \
    src/openglwidget.cpp \
    src/main.cpp \
    src/shader.cpp \
    src/shaderprogram.cpp \
    src/mesh.cpp \
    src/light.cpp \
    src/submesh.cpp \
    src/gameobject.cpp \
    src/makaidebug.cpp

HEADERS  += headers/mainwindow.h \
    headers/openglwidget.h \
    headers/camera.h \
    headers/light.h \
    headers/shaderprogram.h \
    headers/shader.h \
    headers/mesh.h \
    headers/submesh.h \
    headers/gameobject.h \
    headers/makaidebug.h

FORMS    += mainwindow.ui

RESOURCES += \ 
    meshviewer.qrc

INCLUDEPATH += $$PWD/headers

LIBS += -L$$PWD/lib/x86 -lassimp \
    -lopengl32 \
    -lGlu32 \
    -L$$PWD/lib/x86 -lglew32 \
    -lglew32s \
    -lSOIL

DISTFILES += \
    shaders/gourandshader.frag \
    shaders/shader.frag \
    shaders/gourandshader.vert \
    shaders/shader.vert \
    shaders/emission.frag
