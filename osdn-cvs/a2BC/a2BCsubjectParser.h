#ifndef A2BCSUBJECTPARSER_H
#define A2BCSUBJECTPARSER_H

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QTextStream>

class a2BCsubjectParser :  public QWidget
{
    Q_OBJECT

public:
    a2BCsubjectParser(QWidget *parent = 0);
    virtual ~a2BCsubjectParser();

public:
    virtual int  parseSubjectFile(const QString &arFileName);
    virtual void outputSubjectFile(void);

    virtual bool exist(const QString &arFileName);
    virtual bool isPrepare();
    
    virtual void appendData(QString &arFileName, QString &arTitle);

public slots:

signals:
    void  endOutputFile();

public:
    virtual void   clearSubjectFile();    

private:
    virtual bool process_line(QString &arLineData);

private:
    bool             mPrepare;
    QString           mFileName;
    QVector<QString>  mSubjectData;
    
};

#endif // #ifndef #ifndef A2BCSUBJECTPARSER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
