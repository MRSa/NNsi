#ifndef SYNCLOG_H
#define SYNCLOG_H
#include <QWidget>
#include "a2BCotherBrowserFactory.h"
#include "bbsTableParser.h"

class screenData;
class syncLog : public QWidget
{
    Q_OBJECT

public:
    syncLog(QWidget *parent = 0);
    virtual ~syncLog();

signals:
    void updateMessage(const QString &);
    void completedSynchronize(const QString &);
    void updateValue(int value);
    void setMaximum(int value);

public:    
    bool prepare(screenData &arDB);
    bool doSync(int aSyncType, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt);

    QString getSourceBrowserName();
    QString getDestinationBrowserName();

private:
    void moveToUnmanagedLogFile(int aSyncType);
    bool synchronizeMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt);
    bool logCopyMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aRemoveErrorLog, Qt::CheckState aCopySubjectTxt);

private:
    a2BC_AbstractOtherBrowserParser *mpBrowserParser, *mpA2BbrowserParser;
    a2BCotherBrowserFactory          mOtherBrowserFactory;
    bbsTableParser                   mBbsTableParser;
    bool                             mIsReady;
};

#endif    // #ifndef SYNCLOG_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
