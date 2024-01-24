#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "janeBrowserParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  コンストラクタ
 * 
 */
janeBrowserParser::janeBrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // ここでは何もしない
}


/*!
 *  デストラクタ
 * 
 */
janeBrowserParser::~janeBrowserParser()
{
    // ここでは何もしない
}

/*!
 *   ブラウザの名称を応答する
 * 
 */
QString janeBrowserParser::getBrowserName(void)
{
    QString name = "jane";
    return (name);
}

/*!
 *   管理パスの指定...
 * 
 */
void janeBrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
}

/*!
 *   bbstable.htmlの指定...
 * 
 */
void janeBrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    // BBS解析クラス
    mpBbsTableParser = apBbsTable;

    // bbstable.html
    mBbsTable = arBbsTable;
}

/*!
 *   解析の準備...
 * 
 */    
bool janeBrowserParser::prepare(void)
{
    // 変数をクリアする
    clearVariables();

    // 板一覧を準備する
    mBbsParser.prepare(mLogPath, "jane2ch.brd");

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/*!
 *   解析の準備(ディレクトリを掘る...)
 * 
 */    
bool janeBrowserParser::checkDirectory(QString &arPath)
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
        if (fileInfo.isFile() == true)
        {
            QString filePath = fileInfo.absolutePath();
            QString fileName = fileInfo.fileName();
            QString suffix   = fileInfo.suffix();
            if (suffix == "dat")
            {
                QString idxFile = fileName;
                idxFile.replace(QRegExp("dat$"), "idx");
                if (QFile::exists(filePath + "/" + idxFile) == true)
                {
                    // 管理中datファイルだった
                    // (インデックスファイルの解析を実行)
                    parseIndexFile(filePath, idxFile);
                }
            }
        }
    }
    return (true);
}

/*!
 *  janeの未読位置管理ファイルの読み出し...
 * 
 * 
 */
void janeBrowserParser::parseIndexFile(QString &arPath, QString &arFileName)
{
    // bbsmenu.idx と bbsmenu.dat は揃っているので排除する...
    if (arFileName == "bbsmenu.idx")
    {
        return;
    }
    mIndexParser.parseFile(arPath, arFileName);
    return;
}

/*!
 *  datファイルの数を応答する
 * 
 */
int janeBrowserParser::datFileCount(void)
{
    return (mIndexParser.count());
}

/*!
 *   管理外のdatファイルをチェックする
 *    (janeでは当面対応しないので0)
 * 
 */
int  janeBrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/*!
 *   管理外のdatファイル名を取得する
 *    (janeでは当面対応しない)
 * 
 */
QString janeBrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/*!
 *   管理外のdat板Nick名を取得する
 *    (janeでは当面対応しない)
 * 
 */
QString janeBrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/*!
 *   datファイル名(full path)を生成する
 * 
 */
QString janeBrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString datName = mBbsParser.getPath(arNick) + arFileName;
    return (datName);
}

/*!
 *   datファイル名(full path)を生成する
 * 
 */
QString janeBrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString pathName = mBbsParser.getPath(arNick);

    QDir path(mLogPath);
    path.mkpath(pathName);

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QString datName = pathName + fileName + ".dat";

    return (datName);
}

/*!
 *  datファイル名を取得する
 * 
 */
QString janeBrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mIndexParser.count()))
    {
        return ("");
    }
    return (mIndexParser.getDatFileName(aIndex));    
}

/*!
 *  板Nick名を取得する
 * 
 */
QString janeBrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return ("");
    }
    return (mIndexParser.getNickName(aIndex));
}

/*!
 *  未読位置の取得
 * 
 */
int  janeBrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return (1);
    }
    return (mIndexParser.getCurrentMessage(aIndex));
}

/*!
 *  最終更新時間の取得
 * 
 */
qint64 janeBrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return (0);
    }
    QString str = mIndexParser.getLastUpdate(aIndex);

//////////// TODO : 文字列 → 数値変換が必要!!
    return (0);
}

/*
 *   subject.txtのファイル名(full path)を取得する
 * 
 */
QString janeBrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString pathName = mBbsParser.getPath(arNick) + "subject.txt";
    return (pathName);
}

/*!
 *   datファイルを受信する
 * 
 */
bool janeBrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(entry File)", arNick + " | " + arFile, QMessageBox::Ok);

    // ファイルを移動させる
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, true);

    // 管理情報を更新する
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/*!
 *   datファイルをコピーする
 * 
 */
bool janeBrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arSrc + " -> " + arDst + " ", QMessageBox::Ok);

    // ファイルをコピーする
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, true);

    // 管理情報を更新する
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/*!
 *   管理情報を更新する
 * 
 */
bool janeBrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{

    // ファイル情報を取得する
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);
    int    curMsg = aReadPoint;
    if (curMsg == nofMsg)
    {
        curMsg--;
    }
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // 登録済み...更新する
        mIndexParser.setCurrentMessage(index, curMsg);
        mIndexParser.setNofMessage(index, nofMsg);
    }

    QString nick = arNick;
    nick.replace("/", "");
    QString url = mpBbsTableParser->getUrl(nick);
    int pos = url.lastIndexOf("/");
    url = url.left(pos);

    QString pathName = mBbsParser.getPath(arNick);
    pos = pathName.lastIndexOf("/");
    pathName = pathName.left(pos);
    
    // アイテムを追加登録
    mIndexParser.appendData(arNick, url, pathName, arFile, title, nofMsg, curMsg);
    return (true);
}

/*!
 *   あとしまつを実行する (idxファイルを出力する...)
 * 
 */
bool janeBrowserParser::finalize(void)
{
    mIndexParser.outputFile();
    clearVariables();
    return (true);
}

/*!
 *   内部変数ををクリアする
 * 
 */
void janeBrowserParser::clearVariables(void)
{
    //
    //
    //   
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
