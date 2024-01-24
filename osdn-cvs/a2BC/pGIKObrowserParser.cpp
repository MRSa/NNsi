#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "pGIKObrowserParser.h"
#include "pGIKOacquiredParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  コンストラクタ
 * 
 */
pGIKObrowserParser::pGIKObrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // ここでは何もしない
}


/*!
 *  デストラクタ
 * 
 */
pGIKObrowserParser::~pGIKObrowserParser()
{
    // ここでは何もしない
}

/*!
 *   ブラウザの名称を応答する
 * 
 */
QString pGIKObrowserParser::getBrowserName(void)
{
    QString name = "pGIKO";
    return (name);
}

/*!
 *   管理パスの指定...
 * 
 */
void pGIKObrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
}

/*!
 *   bbstable.htmlの指定...
 * 
 */
void pGIKObrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
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
bool pGIKObrowserParser::prepare(void)
{
    mDatNickName.clear();
    mDatFilePath.clear();
    mDatFileName.clear();
    mIdxFilePath.clear();
    mIdxTitle.clear();
    mIdxReadNumber.clear();
    mAcquiredNickName.clear();
    mAcquiredFileName.clear();
    clearAcquiredParser();

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/*!
 *   解析の準備(ディレクトリを掘る...)
 * 
 */    
bool pGIKObrowserParser::checkDirectory(QString &arPath)
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
                int pos  = filePath.lastIndexOf("/dat");
                int pos2 = filePath.lastIndexOf("/", (pos - 1));
                QString nickName = filePath.mid((pos2 + 1), (pos - pos2));
                mDatNickName.append(nickName);
                mDatFilePath.append(filePath);
                mDatFileName.append(fileName);
            }

            // ぽけギコのログ管理データファイル...
            if (fileName == "acquired.txt")
            {
                QString idxFile = filePath + "/acquired.txt";
                int pos = filePath.lastIndexOf("/");
                QString nickName = filePath.mid(pos + 1) + "/";
                parseIndexFile(nickName, idxFile);
            }
        }
    }
    return (true);
}

/*!
 *  ぽけギコの未読位置管理ファイルの読み出し...
 * 
 * 
 */
void pGIKObrowserParser::parseIndexFile(QString &arNickName, QString &arFileName)
{
//    QMessageBox::information(0, "Info(index File)", arNickName + " | " + arFileName, QMessageBox::Ok);
    pGIKOacquiredParser *parser = new pGIKOacquiredParser();
    parser->parseFile(arFileName);

    mAcquiredNickName.append(arNickName);
    mAcquiredFileName.append(arFileName);
    mAcquiredParser.append(parser);

    return;
}

/*!
 * 
 * 
 */
int pGIKObrowserParser::datFileCount(void)
{
    return (mDatFilePath.size());
}

/*!
 *   管理外のdatファイルをチェックする
 * 
 */
int  pGIKObrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/*!
 *   管理外のdatファイル名を取得する
 * 
 */
QString pGIKObrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/*!
 *   管理外のdat板Nick名を取得する
 * 
 */
QString pGIKObrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/*!
 *   datファイル名(full path)を生成する
 * 
 */
QString pGIKObrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString nick = arNick;
    nick.replace("/", "");
    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString datName = mLogPath + pathName + "dat/" + arFileName;
    return (datName);
}

/*!
 *   datファイル名(full path)を生成する
 * 
 */
QString pGIKObrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString nick = arNick;
    nick.replace("/", "");

    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QDir path(mLogPath);
    path.mkpath(mLogPath + pathName + "dat/");

    QString datName = mLogPath + pathName + "dat/" + fileName + ".dat";
    return (datName);
}

/*!
 *  datファイル名を取得する
 * 
 */
QString pGIKObrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileName.size()))
    {
        return ("");
    }
    return (mDatFileName[aIndex]);    
}

/*!
 *  板Nick名を取得する
 * 
 */
QString pGIKObrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFilePath.size()))
    {
        return ("");
    }
    QString filePath = mDatFilePath[aIndex];
    filePath.replace(QRegExp("/dat$"), "");
    int pos = filePath.lastIndexOf(QRegExp("[\\\\/]"));  // '\' か '/' にマッチする...
    pos++;
    return (filePath.mid(pos) + "/");    
}

/*!
 *  未読位置の取得
 * 
 */
int  pGIKObrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFilePath.size()))
    {
        return (1);
    }
    QString nickName = mDatNickName[aIndex];
    int limit = mAcquiredNickName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        if (mAcquiredNickName[loop] == nickName)
        {
            int index = (mAcquiredParser[loop])->exists(mDatFileName[aIndex]);
            if (index < 0)
            {
                return (1);
            }
            return ((mAcquiredParser[loop])->getCurrentMessage(index));
        }
    }
    return (1);
}

/*
 *   subject.txtのファイル名(full path)を取得する
 * 
 */
QString pGIKObrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString nick = arNick;
    nick.replace("/", "");

    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString datName = mLogPath + pathName + "subject.txt";
    return (datName);
}

/*!
 *  最終更新時間の取得
 * 
 */
qint64 pGIKObrowserParser::getLastUpdate(int aIndex)
{
    // ぽけギコは管理していない...0を応答する。
    return (0);
}

/*!
 *   datファイルを受信する
 * 
 */
bool pGIKObrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
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
bool pGIKObrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arNick + " | " + arFile + " - ", QMessageBox::Ok);

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
bool pGIKObrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // ファイル情報を取得する
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);

    pGIKOacquiredParser *parser = 0;
    int index = -1;
    int limit = mAcquiredNickName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        if (mAcquiredNickName[loop] == arNick)
        {
            // 特定できた！
            parser = mAcquiredParser[loop];
            if (parser != 0)
            {
                index = parser->exists(arFile);

            }
            if (index >= 0)
            {
                // 登録済み...更新する
                parser->setCurrentMessage(index, aReadPoint);
                parser->setNofMessage(index, nofMsg);
            }
            break;
        }
    }

    if (index < 0)
    {
        if (parser == 0)
        {
            // acquired.txtを追加する
            path   = getReceiveDatFileName(arNick, arFile);
            target = "dat/" + arFile;
            path.replace(target, "acquired.txt");
            parser = new pGIKOacquiredParser();
            parser->parseFile(path);
            mAcquiredNickName.append(arNick);
            mAcquiredFileName.append(path);
            mAcquiredParser.append(parser);
        }

        // アイテムを追加登録
        parser->appendData(arFile, title, nofMsg, aReadPoint);
    }
    return (true);
}

/*!
 *   あとしまつを実行する (acquired.txt を出力する...)
 * 
 */
bool pGIKObrowserParser::finalize(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = 0; loop < limit; loop++)
   {
        mAcquiredParser[loop]->outputFile();
   }
   return (true);
}

/*!
 *   pGIKOacquiredParserをクリアする
 * 
 */
void pGIKObrowserParser::clearAcquiredParser(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = limit; loop > 0; loop--)
   {
        delete mAcquiredParser[loop];
   }    
   mAcquiredParser.clear();
   
   return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
