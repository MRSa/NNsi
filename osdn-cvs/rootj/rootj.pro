SOURCES	+= main.cpp \
	HttpComm.cpp \
	BoardParser.cpp \
	ThreadParser.cpp
HEADERS	+= HttpComm.h \
	BoardParser.h \
	ThreadParser.h \
	rootjSymbols.h
# ----------------------------------------------------------------------------
#   $Id: rootj.pro,v 1.3 2005/04/24 14:52:59 mrsa Exp $
#
#    Qtのアプリケーション構築用プロジェクトファイル
# ----------------------------------------------------------------------------
unix {
  UI_DIR      = .ui
  MOC_DIR     = .moc
  OBJECTS_DIR = .obj
}


FORMS	= mainapp.ui
CONFIG	+= qt warn_on release
DEFINES	+= QNNSI_DEBUG
FORMS	= mainapp.ui
FORMS	= mainapp.ui
TEMPLATE	=app
LANGUAGE	= C++
#  {語変換ファイルを用意ｷ
TRANSLATIONS = rootj_jp.ts
