#ifndef A2BCFAVORITEPARSER_H
#define A2BCFAVORITEPARSER_H

#include <QWidget>
#include <QString>
#include <QTextStream>
#include "a2BCfavoriteData.h"
/*
 * a2BCfavoriteParser
 * 
 *   a2bのお気に入り管理ファイル
 * 
 */
class a2BCfavoriteParser :  public QWidget
{
    Q_OBJECT

public:
    a2BCfavoriteParser(QWidget *parent = 0);
    virtual ~a2BCfavoriteParser();

public:
    virtual int parseIndexFile(const QString &arFileName);
    virtual void outputIndexFile(bool aRemoveError, bool aRemoveOver);


public slots:

public:
    virtual void   clear();

    void            appendData(const QString &arFileName, QString &arDatFileName, int maxMsg, int curMsg, int status, int level, quint64 lastUpdate, int dummy2, int dummy3, QString &arUrl, QString &arNick, QString &arOption, int usable);

    void            updateUrl(const QString &arBoardNick, const QString &arBoardUrl);

    QString          getIndexFileName(int index);
    QString          getFileName(int index);
    QString          getOptionData(int index);
    QString          getUrl(int index);

    int             getFileStatus(int index);
    int             getNofMessage(int index);
    int             getCurrentMessage(int index);
    int             getFavoriteLevel(int index);
    quint64         getLastUpdate(int index);

    void            setFileStatus(int index, int status);
    void            setNofMessage(int index, int status);
    void            setCurrentMessage(int index, int status);
    void            setLastUpdate(int index);

    void            remove(int index);
    void            markIt(int index);
    int             exists(QString &arNick, QString &arDatFileName);
    int             count(void);

    a2BCfavoriteData *getFavoriteObject(int index);

private:
    virtual bool process_line(const QString &arFileName, QString &arLineData);

private:
/**
    QVector<QString> mIndexFileName;
    QVector<QString> mDatFileName;
    QVector<QString> mUrlData;
    QVector<QString> mNickData;
    QVector<QString> mOptionData;
    
    QVector<int>    mDatFileStatus;
    QVector<int>    mNofMessage;
    QVector<int>    mCurrentMessage;
    QVector<int>    mFavoriteLevel;
    QVector<quint64> mLastUpdate;
    QVector<int>    mDummy2;
    QVector<int>    mDummy3;
    QVector<int>    mUsable;
**/
    QVector<a2BCfavoriteData *> mFavoriteData;
};

#endif // #ifndef #ifndef A2BCFAVORITEPARSER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
