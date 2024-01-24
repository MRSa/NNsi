/*!
 *   実処理クラス
 *
 */
#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QString>
#include <QStatusBar>
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include "qmainwindow.h"
#include "ui_wizard.h"      // *.ui ファイルから自動生成される (ui_*.h)

class screenControl;
class screenWidget : public QWidget
{
    Q_OBJECT

public:
    screenWidget(QWidget *parent = 0);
    virtual ~screenWidget() {};

public:
    bool prepare(QMainWindow      *main,
                 Ui::a2bInstaller *uiInstaller, 
                 screenControl *control);
};

#endif


