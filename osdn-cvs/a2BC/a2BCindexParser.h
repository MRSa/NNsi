#ifndef A2BCINDEXPARSER_H
#define A2BCINDEXPARSER_H

#include <QWidget>
#include <QString>
#include <QTextStream>
#include "a2BCdatFileChecker.h"

class a2BCindexParser :  public QWidget
{
    Q_OBJECT

public:
    a2BCindexParser(QWidget *parent = 0);
    virtual ~a2BCindexParser();

public:
    virtual int parseIndexFile(const QString &arFileName);
    virtual void outputIndexFile(bool aRemoveError, bool aRemoveOver, bool outputSubjectTxt, a2BCdatFileChecker *apDatFileChecker);


public slots:

signals:

public:
    virtual void   clearIndexFile();    

    QString          getIndexFileName(int index);
    QString          getFileName(int index);
    QString          getOptionData(int index);
    QString          getNickName(int index);

    int             exists(const QString &arFileName);
    int             getFileStatus(int index);
    int             getNofMessage(int index);
    int             getCurrentMessage(int index);
    quint64          getLastUpdate(int index);

    void            setFileStatus(int index, int status);
    void            setNofMessage(int index, int status);
    void            setCurrentMessage(int index, int status);
    void            setLastUpdate(int index);

    bool            insertData(QString &arIndexFileName, QString &arDatFileName, QString &arReserve2, QString &arOption, int currentMessage, int nofMessage, qint64 lastUpdate);

private:
    virtual bool process_line(const QString &arFileName, QString &arLineData);

private:
    QVector<QString> mIndexFileName;
    QVector<QString> mDatFileName;
    QVector<QString> mReserve2;
    QVector<QString> mOptionData;
    
    QVector<int>    mDatFileStatus;
    QVector<int>    mNofMessage;
    QVector<int>    mCurrentMessage;
    QVector<quint64> mLastUpdate;
};

#endif // #ifndef #ifndef A2BCINDEXPARSER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
