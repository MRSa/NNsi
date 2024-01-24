#ifndef GIKONAVIIDXPARSER_H
#define GIKONAVIIDXPARSER_H

#include <QStringList>
#include <QWidget>
#include <QVector>
#include <QString>

class GikoNaviIdxParser :  public QWidget
{
    Q_OBJECT

public:
    GikoNaviIdxParser(QString &arNick, QWidget *parent = 0);
    virtual ~GikoNaviIdxParser();

public:
    virtual void  reset();
    virtual int  parseFile(const QString &arFileName, QStringList &arDatFileNameList);
    virtual void outputFile(void);

public slots:

signals:

public:
    int    exists(const QString &arFileName);
    void   updateReadPoint(int index, int currentMessage, int nofMessage, qint64 aLastUpdate);
    bool   appendData(QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage, qint64 aLastUpdate);
    QString getMyNick() { return (mNick); }
    QString getParsedFile() { return (mIndexFileName); }

private:
    virtual bool process_line(const QString &arFileName, int count, QString &arLineData, QStringList &arDatFileNameList);

private:
    QString          mNick;
    QString          mIndexFileName;
    QString          mFileName;
    QStringList      mIndexFileData;
    QVector<QString> mDatFileName;
    QVector<QString> mTitle;
    QVector<int>    mNofMessage;
    QVector<int>    mCurrentMessage;

};

#endif // #ifndef #ifndef GIKONAVIIDXPARSER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2007- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
