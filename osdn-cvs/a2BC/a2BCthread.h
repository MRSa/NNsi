#ifndef A2BCTHREAD_H
#define A2BCTHREAD_H

#include <QThread>

class a2BCthread : public QThread
{
    Q_OBJECT

public:
    a2BCthread(QObject *parent = 0);
    virtual ~a2BCthread();

public slots:
    void setTimer(int id, unsigned long milliSeconds);

signals:
    void timeout(int);

public:
    void run();
};

#endif // #ifndef A2BCTHREAD_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
