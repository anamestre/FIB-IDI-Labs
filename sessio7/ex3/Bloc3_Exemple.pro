TEMPLATE    = app
QT         += opengl

INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH += Model
SOURCES += Model/model.cpp

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp  MyForm.cpp \
        MyGLWidget.cpp 
