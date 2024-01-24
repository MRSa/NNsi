#ifndef A2BCMODEL_H
#define A2BCMODEL_H

#include <QEvent>
#include <QWidget>
#include <QString>
#include "a2BcConstants.h"
#include "screenData.h"
#include "a2BChttpConnection.h"
#include "bbsTableParser.h"
#include "a2BCindexParser.h"
#include "a2BCgettingFile.h"
#include "a2BCfavoriteParser.h"
#include "a2BCbbsTableTrim.h"
#include "a2BCdatFileChecker.h"

class QDir;
class a2BCmodel : public QWidget
{
    Q_OBJECT

public:
    a2BCmodel(screenData *apDB, a2BChttpConnection *apHttp, a2BCindexParser *apParser, QWidget *parent = 0);
    virtual ~a2BCmodel();

protected:
    virtual void customEvent(QEvent *event);

public:
    bool selection_bbsTableFile(QString &arFileName);
    void update_bbsTableFile(QString &arFileName, QString &arUrl);
    void update_a2B_datas(void);

public slots:
    void cancelGetBbsTable();
    void receiveTimeout(int id);
    void completedCommunication(int error);
    void nextCommunication();

signals:
    void updateMessage(const QString &);
    void setTimer(int, unsigned long);
    void startedUpdate();
    void completedUpdate();
    void startedSynchronize();
    void completedSynchronize();
    void updateValue(int value);
    void setMaximum(int value);

private:
    void initializeSelf(void);
    void finishGetBbsTable(void);
    void trimBbsTable(bbsTableParser &arBbsTable);
    void resetTrimBbsTable(void);
    void checkDirectoryList(QDir &arBaseDirectory);

    int  parseA2Bindex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick, int aOffset);
    bool prepareA2BLogDirectory(bbsTableParser &arBbsTable, QString &arBaseDirectory);
    bool confirmStartUpdate(void);
    void parseOldThreadFile(QString &arBaseDirectory, QString &arBoardNick);
    void parseSubjectTxtIndex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick);

private:
    screenData                *mpDB;
    a2BChttpConnection        *mpHttp;
    a2BCindexParser           *mpIndexParser;

    QString                    mBaseDirectory;
    int                       mIndexCount;
    int                       mStartedGetting;
    int                       mGetUpdated;
    int                       mGetFailure;
    int                       mNotModified;
    QVector<QString>           mNick;
    QVector<a2BCgettingFile *> mGettingFileList;
    QVector<int>              mGettingFileIndex;

    a2BCbbsTableTrim           mBbsTableTrim;
    a2BCfavoriteParser         mFavoriteParser;
    a2BCdatFileChecker         mDatFileChecker;
};

#endif  // #ifndef A2BCMODEL_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
