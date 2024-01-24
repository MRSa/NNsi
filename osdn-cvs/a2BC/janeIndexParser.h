#ifndef JANEINDEXPARSER_H
#define JANEINDEXPARSER_H

#include <QVector>
#include <QString>

class janeIndexParser
{

public:
    janeIndexParser();
    virtual ~janeIndexParser();

public:
    virtual bool parseFile(const QString &arPath, const QString &arIdxFileName);
    virtual void outputFile(void);

public:
    void   prepare(void);
    int    exists(const QString &arFileName);
    int    count(void);

    int    getNofMessage(int index);
    int    getCurrentMessage(int index);
    QString getTitle(int index);
    QString getDatFileName(int index);
    QString getNickName(int index);
    QString getLastUpdate(int index);

    void   setNofMessage(int index, int status);
    void   setCurrentMessage(int index, int status);
    void   setTitle(int index, QString &arTitleName);
    
    bool   appendData(QString &arNick, QString &arUrl, QString &arFilePath, QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage);

private:
    void process_line(const int count, QString &arLineData);
    void decideBoardNick(QString &arUrl);
    void outputFileMain(int index);
    void clearVariables(void);

private:
    QVector<QString> mFilePath;
    QVector<QString> mIdxFileName;
    QVector<QString> mTitle;
    QVector<QString> mLastUpdate;
    QVector<QString> mNofMsg;
    QVector<QString> mCurMsg;
    QVector<QString> mDummy1;
    QVector<QString> mUrl;
    QVector<QString> mDummy2;
    QVector<QString> mDummy3;
    QVector<QString> mDummy4;
    QVector<QString> mDummy5;
    QVector<QString> mDummy6;
    QVector<QString> mUpdate;
    QVector<QString> mDummy7;
    QVector<QString> mDummy8;
    QVector<QString> mDummy9;

    QVector<QString> mNick;
};

#endif // #ifndef JANEINDEXPARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
