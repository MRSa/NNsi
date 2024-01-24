#ifndef BBSTABLE_PARSER_H
#define BBSTABLE_PARSER_H

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QMap>


class bbsTableParser : public QWidget
{
    Q_OBJECT

public:
    bbsTableParser(QWidget *parent = 0);
    ~bbsTableParser();

public:
    bool       prepare(QString &arFileName);
    int        numberOfBoards();
    QString    getUrl(QString &arNick);
    QByteArray getData(QString &arNick);

private:
    bool                  mParsed;
    QMap<QString, QString> mUrlMap;
    QMap<QString, QByteArray> mBbsDataMap;
};

#endif // #ifndef BBSTABLE_PARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */

