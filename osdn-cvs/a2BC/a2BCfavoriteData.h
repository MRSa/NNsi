#ifndef A2BCFAVORITEDATA_H
#define A2BCFAVORITEDATA_H

#include <QWidget>
#include <QString>
/*
 * a2BCfavoriteParser
 * 
 *   a2bのお気に入り管理ファイル
 * 
 */
class a2BCfavoriteData :  public QWidget
{
    Q_OBJECT

public:
    a2BCfavoriteData(QString &arIndexFileName, QString &arDatFileName, int maxMsg, int curMsg, int status, int level, quint64 lastUpdate, int dummy2, int dummy3, QString &arUrl, QString &arNick, QString &arOption, int usable, QWidget *parent = 0);
    virtual ~a2BCfavoriteData();

public slots:

signals:

public:
    QString  getIndexFileName();
    QString  getDatFileName();
    QString  getUrl();
    QString  getNickName();
    QString  getOption();
    int     getFileStatus();
    int     getNofMessage();
    int     getCurrentMessage();
    int     getFavoriteLevel();
    quint64 getLastUpdate();
    int     getDummy2();
    int     getDummy3();
    int     getUsable();

public:
    void setIndexFileName(QString &arIndexFileName);
    void setDatFileName(QString &arFileName);
    void setUrl(QString &arUrl);
    void setNickName(QString &arNickName);
    void setOption(QString &arOption);
    void setFileStatus(int status);
    void setNofMessage(int nofMessage);
    void setCurrentMessage(int curMessage);
    void setFavoriteLevel(int level);
    void setLastUpdate(quint64 lastUpdate);
    void setDummy2(int dummy2);
    void setDummy3(int dummy3);
    void setUsable(int usable);

private:
    QString mIndexFileName;
    QString mDatFileName;
    QString mUrlData;
    QString mNickData;
    QString mOptionData;
    
    int     mDatFileStatus;
    int     mNofMessage;
    int     mCurrentMessage;
    int     mFavoriteLevel;
    quint64  mLastUpdate;
    int     mDummy2;
    int     mDummy3;
    int     mUsable;
};

#endif // #ifndef #ifndef A2BCFAVORITEDATA_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
