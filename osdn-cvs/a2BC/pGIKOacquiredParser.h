#ifndef PGIKOACQUIREDPARSER_H
#define PGIKOACQUIREDPARSER_H

#include <QWidget>
#include <QVector>
#include <QString>

class pGIKOacquiredParser :  public QWidget
{
    Q_OBJECT

public:
    pGIKOacquiredParser(QWidget *parent = 0);
    virtual ~pGIKOacquiredParser();

public:
    virtual int  parseFile(const QString &arFileName);
    virtual void outputFile(void);

public slots:

signals:

public:
    virtual void   clearFile(void);    
    int             exists(const QString &arFileName);
    int             count(void);

    int             getNofMessage(int index);
    int             getCurrentMessage(int index);
    QString          getTitle(int index);

    void            setNofMessage(int index, int status);
    void            setCurrentMessage(int index, int status);
    void            setTitle(int index, QString &arTitleName);
    
    bool            appendData(QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage);

private:
    virtual bool process_line(const QString &arFileName, QString &arLineData);

private:
    QString          mFileName;
    QVector<QString> mDatFileName;
    QVector<QString> mTitle;
    QVector<int>    mNofMessage;
    QVector<int>    mCurrentMessage;

};

#endif // #ifndef #ifndef PGIKOACQUIREDPARSER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
