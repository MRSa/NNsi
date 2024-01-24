#ifndef A2B_HTTP_CONNECTION_H
#define A2B_HTTP_CONNECTION_H

#include <QWidget>
#include <QtNetwork/QHttp>
#include <QFile>

class a2BChttpConnection : public QWidget
{
    Q_OBJECT

public:
    a2BChttpConnection(QHttp *apHttp, QWidget *parent = 0);
    virtual ~a2BChttpConnection();

public:
    void getFileFromUrl(QString &arFileName, QString &arUrl, bool isPartGet);
    void setCookie(QString &arCookie);
    void setUserAgent(QString &arUserAgent);
    bool isCommunicating();

public slots:
    void done(bool error);
    void readyRead(const QHttpResponseHeader &resp);

signals:
    void completedCommunication(int, QString &);
    void updateMessage(const QString &);

private:
    void cleanup();

private:
    QHttp              *mpHttp;
    QHttpRequestHeader *mpHeader;
    QFile              *mpFile;
    int                 mHttpGetId;
    QString             mFileName;
    QString             mCookie;
    QString             mUserAgent;
    bool                mFirstGet;
    bool                mIsPartGet;
    int                 mGetResult;
};

#endif  // #define A2B_HTTP_CONNECTION_H
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
