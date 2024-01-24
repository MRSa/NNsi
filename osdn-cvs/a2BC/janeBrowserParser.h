#ifndef JANEBROWSERPARSER_H
#define JANEBROWSERPARSER_H
#include <QString>
#include <QVector>
#include "a2BC_AbstractOtherBrowserParser.h"
#include "janeIndexParser.h"
#include "janeBbsParser.h"

/*
 * Jane系のデータ解析クラス...
 * 
 */
class QString;
class bbsTableParser;
class janeBrowserParser : public a2BC_AbstractOtherBrowserParser
{

public:
    janeBrowserParser();
    virtual ~janeBrowserParser();

public:
    virtual QString getBrowserName(void);          // ブラウザ名を応答する
    virtual void setPath(QString &arPath);         // ログ格納ディレクトリ(のルートディレクトリ)を設定
    virtual void setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable); // BBSTABLE.HTML(というか板一覧ファイル)の管理ファイルを指定
    virtual bool prepare(void);                   // ログデータの準備。。。
    virtual int  datFileCount(void);              // ログデータ数を応答する

    virtual int     checkUnmanagedDatFile(void);                 // 管理外のdatファイルをチェックする
    virtual QString getUnmanagedDatFileName(int aIndex);       // 管理外のdatファイル名を取得する
    virtual QString getUnmanagedDatNickName(int aIndex);       // 管理外のdat板ニックネームを取得する
    virtual QString getFullPathDatFileName(QString &arNick, QString &arFileName);  // datファイル名を生成する
    virtual QString getReceiveDatFileName(QString &arNick, QString &arFileName);   // 受信用datファイル名を生成する
    virtual QString getDatFileName(int aIndex);                // datファイル名を取得する
    virtual QString getBoardNick(int aIndex);                  // datファイルの存在する板nickを取得する
    virtual bool isSameLogFormat(void) {return (true); };   // ログフォーマットがa2Bと同じか？
    virtual int  getReadPoint(int aIndex);                    // 読んだ場所を応答する
    virtual qint64 getLastUpdate(int aIndex);                  // 最終更新時間を取得する

    virtual QString getSubjectTxtFilePath(QString &arNick);         // subject.txtのファイル名(full path)を取得する
    virtual bool receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // datファイルを受信する
    virtual bool copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // datファイルを受信する
    virtual bool finalize(void);                             // 後始末する。。。

private:
    void clearVariables(void);
    bool checkDirectory(QString &arPath);
    void parseIndexFile(QString &arPath, QString &arFileName);

    bool updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);

private:
    bbsTableParser      *mpBbsTableParser;
    QString              mBbsTable;
    QString              mLogPath;
    janeIndexParser      mIndexParser;
    janeBbsParser        mBbsParser;

};

#endif // #ifndef JANEBROWSERPARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
