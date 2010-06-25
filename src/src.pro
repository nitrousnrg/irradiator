SOURCES += irradiator.cpp \
           main.cpp \
 contour.cpp \
 calculos.cpp
HEADERS += irradiator.h \
 contour.h \
 calculos.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/irradiator
RESOURCES = application.qrc
INCLUDEPATH += /usr/include/qwt-qt4/

LIBS += -L/usr/lib \
  -lqwt-qt4

QT += qt3support

