#
#  a2B-wxinst.pro
#
QT += network
TEMPLATE = app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += screenControl.h screenData.h screenWidget.h
FORMS += wizard.ui
SOURCES += main.cpp screenControl.cpp screenData.cpp screenWidget.cpp
TRANSLATIONS += translate_jp.ts
