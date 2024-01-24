/*!
 *   a2Bのログ読み書きクラス
 * 
 * 
 */
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "a2BcConstants.h"
#include "a2BbrowserParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  コンストラクタ
 * 
 */
a2BbrowserParser::a2BbrowserParser()
 : mpBbsTableParser(0), mCount(0), mUnmanagedCount(0)
{
    // なにもしない...
}

/*!
 *  デストラクタ
 * 
 */
a2BbrowserParser::~a2BbrowserParser()
{
    // なにもしない...
}

/*!
 *   ブラウザの名称を応答する
 * 
 */
QString a2BbrowserParser::getBrowserName(void)
{
    QString name = "a2B";
    return (name);
}

/*!
 *   管理パスの指定...
 *   (ログファイルを格納するルートディレクトリ名)
 */
void a2BbrowserParser::setPath(QString &arPath)
{
    mBaseDirectory = arPath;
    return;
}

/*!
 *   bbstable解析クラス（とbbstable.html）の指定...
 * 
 */
void a2BbrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    mpBbsTableParser = apBbsTable;
    return;
}

/*!
 *   解析の準備...
 * 
 */    
bool a2BbrowserParser::prepare(void)
{
    /// データをクリアする...
    mUnmanagedDatFilePath.clear();
    mUnmanagedDatFileName.clear();
    mFavoriteParser.clear();
    mIndexParser.clearIndexFile();
    mCount = 0;

    /// お気に入りデータを読み込む...
    mFavoriteParser.parseIndexFile(mBaseDirectory + "/favorite.idx");

    /// ディレクトリを検索して、ログファイルを抽出する
    bool ret = checkDirectory(mBaseDirectory);
    return (ret);
}

/*!
 *   解析の準備(ディレクトリを検索...)
 * 
 */    
bool a2BbrowserParser::checkDirectory(QString &arPath)
{
    QDir targetDir(arPath);
    QFileInfoList fileList = targetDir.entryInfoList();
    for (QFileInfoList::const_iterator ite = fileList.constBegin(); ite != fileList.constEnd(); ite++)
    {
        QFileInfo fileInfo = (*ite);
        if (fileInfo.isDir() == true)
        {
            QString filePath = fileInfo.absoluteFilePath();
            if (filePath.endsWith(".") != true)
            {
                checkDirectory(filePath);
            }
        }

        // ファイルかどうかチェック
        if (fileInfo.isFile() == true)
        {
            // a2Bのログ管理データファイル...
            if ( fileInfo.fileName() == "a2b.idx")
            {
                QString idxFile = fileInfo.absolutePath() + "/a2b.idx";
                parseIndexFile(idxFile);
            }
        }
    }
    return (true);
}

/*!
 *   管理外のdatファイルをチェックする
 * 
 */
int  a2BbrowserParser::checkUnmanagedDatFile(void)
{
//    QMessageBox::information(0, "checkUnmanagedDatFile()", mBaseDirectory, QMessageBox::Ok);

    /// ディレクトリを検索して、ログファイルを抽出する
    mUnmanagedCount = 0;
    checkUnmanagedFiles(mBaseDirectory);

    return (mUnmanagedCount);
}

/*!
 *  管理されていないdatファイルを検出する(メインの処理)
 * 
 */
void  a2BbrowserParser::checkUnmanagedFiles(QString &arPath)
{
    QDir targetDir(arPath);
    QFileInfoList fileList = targetDir.entryInfoList();
    for (QFileInfoList::const_iterator ite = fileList.constBegin(); ite != fileList.constEnd(); ite++)
    {
        QFileInfo fileInfo = (*ite);
        if (fileInfo.isDir() == true)
        {
            QString filePath = fileInfo.absoluteFilePath();
            if (filePath.endsWith(".") != true)
            {
                checkUnmanagedFiles(filePath);
            }
        }
        if (fileInfo.isFile() == true)
        {
            QString filePath = fileInfo.absolutePath();
            QString fileName = fileInfo.fileName();
            QString suffix   = fileInfo.suffix();
            if (suffix == "dat")
            {
                bool unmanaged = false;
                int index = mIndexParser.exists(fileName);
                if (index == -1)
                {
                    unmanaged = true;
                }
                else
                {
                    int nofMsg = mIndexParser.getNofMessage(index);
                    if (nofMsg > 1000)
                    {
                        unmanaged = true;
                    }
                }
                if (index == -1)
//                if (unmanaged == true)
                {
                    // ログが管理されていなかった...
                    mUnmanagedDatFilePath.append(filePath);
                    mUnmanagedDatFileName.append(fileName);
                    mUnmanagedCount++;
                }
            }
        }
    }
    return;    
}

/*!
 *  データ管理ファイルの読み出し...
 * 
 * 
 */
void a2BbrowserParser::parseIndexFile(QString &arFileName)
{
//    QMessageBox::information(0, "Info(index File)", arFileName, QMessageBox::Ok);
    mCount = mCount + mIndexParser.parseIndexFile(arFileName);
    return;   
}

/*!
 *   データファイルの数を応答する
 * 
 */
int a2BbrowserParser::datFileCount(void)
{
    return (mCount);
}

/*!
 *    管理外のdatファイル名を取得する
 * 
 */
QString a2BbrowserParser::getUnmanagedDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mUnmanagedCount))
    {
        return ("");
    }
    return (mUnmanagedDatFileName[aIndex]);
}


/*
 *     管理外のdat板ニックネームを取得する
 * 
 */
QString a2BbrowserParser::getUnmanagedDatNickName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mUnmanagedCount))
    {
        return ("");
    }
    QString nick = mUnmanagedDatFilePath[aIndex];
    QString baseDir = mBaseDirectory.toLower();
    nick = nick.toLower();
    nick.replace(baseDir, "");
    return (nick + "/");
}

/*!
 *  datファイルのfull-pathを取得する
 * 
 */
QString a2BbrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    return (mBaseDirectory + arNick + arFileName);
}


/*!
 *   datファイル名(full path)の受信用ファイル名を生成する
 * 
 */
QString a2BbrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    QString fullPath = mBaseDirectory + arNick + fileName + ".dat";
    return (fullPath);
}

/*!
 *  datファイル名を取得する
 * 
 */
QString a2BbrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return ("");
    } 

    QString fileName = mIndexParser.getFileName(aIndex);
    return (fileName);
}

/*!
 *  板Nick名を取得する
 * 
 */
QString a2BbrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return ("");
    } 

    QString nick = mIndexParser.getNickName(aIndex);
    return (nick + "/");
}

/*!
 *  未読位置の取得
 * 
 */
int  a2BbrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return (1);
    } 

    int curMsg = mIndexParser.getCurrentMessage(aIndex);
    return (curMsg);
}

/*!
 *  最終更新時間の取得
 * 
 */
qint64 a2BbrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return (0);
    } 

    qint64 lastUpdate = mIndexParser.getLastUpdate(aIndex);
    return (lastUpdate);
}

/*
 *   subject.txtのファイル名(full path)を取得する
 * 
 */
QString a2BbrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString fullPath = mBaseDirectory + arNick + "subject.txt";
    return (fullPath);
}

/*!
 *   datファイルを受信する
 * 
 */
bool a2BbrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // ディレクトリを掘る
    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    // datファイルを移動する
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, true);

    // datファイルが管理されているかどうか確認する
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // 管理されているdatファイルだった...
        if (aReadPoint > 0)
        {
            mIndexParser.setCurrentMessage(index, aReadPoint);
            mIndexParser.setFileStatus(index, A2BCSTATUS_UPDATE);
        }
    }
    else
    {
        // 管理されていないdatファイルだった...登録する
        insertData(arNick, arFile, aReadPoint, aLastUpdate);
    }
    
    return (true);
}

/*!
 *   datファイルをコピーする
 * 
 */
bool a2BbrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // ディレクトリを掘る
    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    // datファイルをコピーする
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, true);

    // datファイルが管理されているかどうか確認する
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // 管理されているdatファイルだった...
        if (aReadPoint > 0)
        {
            mIndexParser.setCurrentMessage(index, aReadPoint);
            mIndexParser.setFileStatus(index, A2BCSTATUS_UPDATE);
        }
    }
    else
    {
        // 管理されていないdatファイルだった...登録する
        insertData(arNick, arFile, aReadPoint, aLastUpdate);
    }

    return (true);
}

/*!
 *   管理ファイルへ登録する
 * 
 */
bool a2BbrowserParser::insertData(QString &arNick, QString &arFile, int aReadPoint, qint64 aLastUpdate)
{
    //  QMessageBox::information(0, "insertData()", arNick + " | " + arFile, QMessageBox::Ok);

    datFileUtil util;
    QString target = mBaseDirectory + arNick;
    int pos = target.lastIndexOf("/");
    QString path = target.left(pos);
    int nofMsg = util.checkResCount(path, arFile);
    int curMsg = aReadPoint;
    if (curMsg < 1)
    {
        curMsg = 1;
    }
    QString title   = util.pickupDatFileTitle(path, arFile);
    QString idxFile = mBaseDirectory + arNick + "a2b.idx";
    QString option  = "0";
    mIndexParser.insertData(idxFile, arFile, option, title, curMsg, nofMsg, aLastUpdate);
    return (false);
}

/*!
 *   あとしまつを実行する
 * 
 */
bool a2BbrowserParser::finalize(void)
{
    // "a2b.idx" を出力する。。。
    mIndexParser.outputIndexFile(false, false, false, 0);
    return (true);    
}
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
