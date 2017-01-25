TEMPLATE    = app
QT         += opengl

INCLUDEPATH +=  C:\Users\Carlos\Desktop\IDI3\IDI
INCLUDEPATH +=  C:\Users\Carlos\Desktop\IDI3\IDI\Model

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp  MyForm.cpp \
        MyGLWidget.cpp
SOURCES += C:\Users\Carlos\Desktop\IDI3\IDI\Model\model.cpp

DISTFILES += \
    shaders/fragshad.frag \
    shaders/vertshad.vert
