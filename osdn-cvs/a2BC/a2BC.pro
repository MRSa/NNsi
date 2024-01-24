#
#  a2BC.pro
#
#
TEMPLATE =   app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += .
QT +=   network  xml

QTPLUGIN     += qjpcodecs

QMAKE_LFLAGS +=   -L$$[QT_INSTALL_PLUGINS]/codecs
CONFIG += release static plugin

LIBS +=  -lqjpcodecs4

HEADERS += \
  a2BCversion.h \
  a2BC_AbstractOtherBrowserParser.h \
  a2BCbbsTableTrim.h \
  a2BcConstants.h \
  a2BCparam.h \
  a2BCfavoriteParser.h \
  a2BCgettingFile.h \
  a2BChttpConnection.h \
  a2BCindexParser.h \
  a2BCmodel.h \
  a2BCsubjectParser.h \
  a2BCthread.h \
  a2BCdatFileChecker.h \
  datFileUtil.h \
  bbsTableParser.h \
  screenControl.h \
  screenData.h \
  a2BCotherBrowserFactory.h \
  syncLog.h \
  pGIKObrowserParser.h \
  pGIKOacquiredParser.h \
  janeBrowserParser.h \
  janeIndexParser.h \
  janeBbsParser.h \
  a2BbrowserParser.h \
  a2BCfavoriteEditor.h \
  a2BCfavoriteBbsEditor.h \
  a2BCfavoriteData.h \
  a2BCfavoriteItemModel.h \
  a2BCfavoriteItemDelegate.h \
  a2BCfavoriteBbsData.h \
  GikoNaviBrowserParser.h \
  GikoNaviIdxParser.h \


FORMS +=   a2BC.ui addBoard.ui
SOURCES += \
  a2BCbbsTableTrim.cpp \
  a2BCparam.cpp \
  a2BCfavoriteParser.cpp \
  a2BCgettingFile.cpp \
  a2BChttpConnection.cpp \
  a2BCindexParser.cpp \
  a2BCmodel.cpp \
  a2BCsubjectParser.cpp \
  a2BCthread.cpp \
  a2BCdatFileChecker.cpp \
  datFileUtil.cpp \
  bbsTableParser.cpp \
  main.cpp \
  screenControl.cpp \
  screenData.cpp \
  a2BCotherBrowserFactory.cpp \
  syncLog.cpp \
  pGIKObrowserParser.cpp \
  pGIKOacquiredParser.cpp \
  janeBrowserParser.cpp \
  janeBbsParser.cpp \
  janeIndexParser.cpp \
  a2BbrowserParser.cpp \
  a2BCfavoriteEditor.cpp \
  a2BCfavoriteBbsEditor.cpp \
  a2BCfavoriteData.cpp \
  a2BCfavoriteItemModel.cpp \
  a2BCfavoriteItemDelegate.cpp \
  a2BCfavoriteBbsData.cpp \
  GikoNaviBrowserParser.cpp \
  GikoNaviIdxParser.cpp \

TRANSLATIONS += a2BC_jp.ts
