#ifndef JANEBBSPARSER_H
#define JANEBBSPARSER_H

#include <QVector>
#include <QString>
#include <QMap>

class janeBbsParser
{

public:
    janeBbsParser();
    virtual ~janeBbsParser();

public:
    virtual void   prepare(const QString &arBasePath, const QString &arFileName);
    virtual QString getPath(const QString &arNick);

private:
    bool parseFile(const QString &arFileName);
    void process_line(QString &arLineData);

private:
   QString mBasePath;
   QString mCategory;

   QMap<QString, QString> mPathMap;

};
#endif  // #ifndef JANEBBSPARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
