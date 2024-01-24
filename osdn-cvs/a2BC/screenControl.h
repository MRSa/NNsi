#ifndef SCREENCONTROL_H
#define SCREENCONTROL_H

#include <QWidget>
#include <QProgressBar>
#include "a2BcConstants.h"
#include "ui_a2BC.h"
#include "screenData.h"
#include "a2BCmodel.h"
#include "a2BChttpConnection.h"
#include "a2BCindexParser.h"
#include "a2BC_AbstractOtherBrowserParser.h"
#include "a2BCparam.h"
#include "syncLog.h"
#include "a2BCfavoriteEditor.h"
#include "a2BCfavoriteBbsEditor.h"

class a2BCthread;
class screenControl : public QWidget
{
    Q_OBJECT

public:
    screenControl(Ui::a2BCMain *apForm, QString &execPath, QWidget *parent = 0);
    virtual ~screenControl();

public:
    void completedGetFile (bool error, QString &arFileName);
    void finishApp(void);

public slots:
    void selection_otherBrowserDirectory();
    void selection_bbsTableFile();
    void select_updateBbsTableFile();
    void executeUpdate();
    void executeSync();
    void cancelUpdate();

    void changed_bbsTableFile();
    void changed_bbsTableUrl();
    void changed_interval();

    void readBbsTableInformations();
    void savedBbsEditor();

    void updateMessage(const QString &arMessage);
    void completedCommunication(int error, QString &arMessage);
    void completedSynchronize(const QString &arMessage);

    void startedUpdate();
    void completedUpdate();

    void show_aboutQt(bool checked);
    void show_about_a2BC(bool checked);

signals:
    void nextCommunication();
    void completedCommunication(int);
    void completedSynchronize();

private:
    void createClasses(QString &execPath);
    void deleteClasses(void);
    void initializeDatas(void);
    void connectSignals(void);
    void updateFormData(void);

private:
    Ui::a2BCMain       *mpForm;
    screenData         *mpDB;
    a2BCmodel          *mpModel;
    a2BChttpConnection *mpHttpConn;
    QHttp              *mpHttpMain;
    a2BCthread         *mpTimer;
    a2BCindexParser    *mpIndexParser;
    a2BCparam           mParameterLoader;
    syncLog             mSyncLog;
    a2BCfavoriteEditor  mFavoriteEditor;
    a2BCfavoriteBbsEditor mFavoriteBbsEditor;
    QProgressBar        mProgressBar;
};

#endif
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
