#ifndef A2BCBBSTRIM_H
#define A2BCBBSTRIM_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QStandardItemModel>

/**
 *   a2BCbbsTableTrim
 * 
 * 
 */
class QTextCodec;
class QTextStream;
class a2BCfavoriteBbsData;
class a2BCbbsTableTrim : public QWidget
{
    Q_OBJECT

public:
    a2BCbbsTableTrim(QWidget *parent = 0);
    virtual ~a2BCbbsTableTrim();

public:
    void resetTrimmer(void);
    bool prepareTrimmer(QString &arFileName);
    void appendTrimmer(QByteArray &arData);
    bool outputTrimmer(void);
    QStandardItemModel *getTrimmerModel() { return (&mItemModel); };
    bool readA2BLogBoards(const QString &arFileName);
    void clearLogBoardData();
    void entryBbsData(QString &boardName, QString &boardUrl, QString &sortKey);

public slots:

signals:

private:
    void appendBoardUrlData(const QString &lineData);
    QTextCodec *setCodec(QTextStream &arOutputStream);
    void outputA2BCcategory(QTextStream &arOutputStream, QTextCodec *codec);
    void outputA2Bcategory_noCodec(QTextStream &arOutputStream);

private:
    QString             mBbsTableFileName;  // 板一覧ファイル名 (full path)
    QVector<QByteArray> mBbsTableAppend;    // 追加するデータ

    QVector<a2BCfavoriteBbsData *> mFavoriteBbsData;  // 追加されているデータ
    QStandardItemModel  mItemModel;                   // 
};

#endif // #ifndef #ifndef A2BCBBSTRIM_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
