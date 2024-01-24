#ifndef SCREENDATA_H
#define SCREENDATA_H

#include <Qt>
#include <QString>
#include "a2BcConstants.h"

class screenData
{
public:
    screenData();
    virtual ~screenData() {};

public:
    QString getBbsTableUrl(void);
    void setBbsTableUrl(const QString &arUrl);

    QString getBbsTableFileName(void);
    void setBbsTableFileName(const QString &arFileName);

    QString getOtherBrowserDirectory(void);
    void setOtherBrowserDirectory(const QString &arDirectory);

    Qt::CheckState getUpdateSubjectTxt(void);
    void setUpdateSubjectTxt(Qt::CheckState yesNo);

    Qt::CheckState getEnableDownThread(void);
    void setEnableDownThread(Qt::CheckState yesNo);

    Qt::CheckState getAddThreadToIndex(void);
    void setAddThreadToIndex(Qt::CheckState yesNo);

    Qt::CheckState getAddToBbsTable(void);
    void setAddToBbsTable(Qt::CheckState yesNo);

    Qt::CheckState getDisableGetPart(void);
    void setDisableGetPart(Qt::CheckState yesNo);

    Qt::CheckState getRemoveErrorMessage(void);
    void setRemoveErrorMessage(Qt::CheckState yesNo);

    Qt::CheckState getRemoveOverMessage(void);
    void setRemoveOverMessage(Qt::CheckState yesNo);

    Qt::CheckState getBackupErrorLog(void);
    void setBackupErrorLog(Qt::CheckState yesNo);

    Qt::CheckState getBackupOverLog(void);
    void setBackupOverLog(Qt::CheckState yesNo);

    Qt::CheckState getCopySubjectTxt(void);
    void setCopySubjectTxt(Qt::CheckState yesNo);

    int getReceiveInterval(void);
    void setReceiveInterval(int interval);

    int getOtherBrowserType(void);
    void setOtherBrowserType(int browserType);

    int getSynchronizeType(void);
    void setSynchronizeType(int browserType);


private:
    QString           bbsTableUrl;
    QString           bbsTableFileName;
    QString           otherBrowserDirectory;
    Qt::CheckState    updateSubjectTxt;
    Qt::CheckState    enableDownThread;
    Qt::CheckState    addThreadToIndex;
    Qt::CheckState    addToBbsTable;
    Qt::CheckState    disableGetPart;
    Qt::CheckState    removeErrorThreadMessage;
    Qt::CheckState    removeOverThreadMessage;
    Qt::CheckState    backupErrorLog;
    Qt::CheckState    backupOverLog;
    Qt::CheckState    copySubjectTxt;
    int               getInterval;
    int               otherBrowserType;
    int               synchronizeType;
};

#endif
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
