#ifndef A2BCPARAM_H
#define A2BCPARAM_H

#include <QWidget>
#include <QString>
#include "screenData.h"

class a2BCparam : public QWidget
{
    Q_OBJECT

public:
    a2BCparam(QWidget *parent = 0);
    virtual ~a2BCparam();

public:
    bool loadFromFile(QString &execPath, screenData *arDB);
    bool saveToFile(void);

private:
    bool setParameter(QString name, QString type, QString value);

private:
    bool        mLoaded;
    bool        mItemParam;
    QString      mPath;
    screenData  *mpDB;

};

#endif // #ifndef A2BCPARAM_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
