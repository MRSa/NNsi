#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "GikoNaviBrowserParser.h"
#include "GikoNaviIdxParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/**
 *  コンストラクタ
 * 
 */
GikoNaviBrowserParser::GikoNaviBrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // ここでは何もしない
}


/**
 *  デストラクタ
 * 
 */
GikoNaviBrowserParser::~GikoNaviBrowserParser()
{
    // ここでは何もしない
}

/**
 *   ブラウザの名称を応答する
 * 
 */
QString GikoNaviBrowserParser::getBrowserName(void)
{
    QString name = "GikoNavi";
    return (name);
}

/**
 *   管理パスの指定...
 * 
 */
void GikoNaviBrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
    //QMessageBox::information(0, "LOG PATH", mLogPath, QMessageBox::Ok);
}

/**
 *   bbstable.htmlの指定...
 * 
 */
void GikoNaviBrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    // BBS解析クラス
    mpBbsTableParser = apBbsTable;

    // bbstable.html
    mBbsTable = arBbsTable;
}

/**
 *   解析の準備...
 * 
 */    
bool GikoNaviBrowserParser::prepare(void)
{
    clearAcquiredParser();

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/**
 *   解析の準備(ディレクトリを掘る...)
 * 
 */    
bool GikoNaviBrowserParser::checkDirectory(QString &arPath)
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
            // ログ管理データファイル...
            if ( fileInfo.fileName() == "Folder.idx")
            {
                QString filePath = fileInfo.absolutePath();
                int pos = filePath.lastIndexOf("/");
                QString nickName = filePath.mid(pos + 1);
                QString idxFile = fileInfo.absolutePath() + "/Folder.idx";
                parseIndexFile(nickName, idxFile);
            }
        }
    }
    return (true);
}

/**
 *  ギコナビの未読位置管理ファイルの読み出し...
 * 
 * 
 */
void GikoNaviBrowserParser::parseIndexFile(QString &arNickName, QString &arFileName)
{
    //QMessageBox::information(0, "Info(index File)", arNickName + " | " + arFileName, QMessageBox::Ok);
    GikoNaviIdxParser *parser = new GikoNaviIdxParser(arNickName);
    parser->parseFile(arFileName, mDatFileList);
    mAcquiredParser.append(parser);
    return;
}

/**
 *  ギコナビが管理しているdatファイルの数を応答する
 * 
 */
int GikoNaviBrowserParser::datFileCount(void)
{
    return (mDatFileList.size());
}

/**
 *   管理外のdatファイルをチェックする
 * 
 */
int  GikoNaviBrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/**
 *   管理外のdatファイル名を取得する
 * 
 */
QString GikoNaviBrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/**
 *   管理外のdat板Nick名を取得する
 * 
 */
QString GikoNaviBrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/**
 *   datファイル名(full path)を生成する
 * 
 */
QString GikoNaviBrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString datName = mLogPath + "/2ch/" + arNick + arFileName;
    return (datName);
}

/*!
 *   datファイル名(full path)を生成する
 * 
 */
QString GikoNaviBrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QDir path(mLogPath);
    path.mkpath(mLogPath + "/2ch/" + arNick);

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QString datName = mLogPath + "/2ch/" + arNick + fileName + ".dat";
    return (datName);
}

/**
 *  datファイル名を取得する
 * 
 */
QString GikoNaviBrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return ("");
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    
    return (dataList.at(1));    
}

/**
 *  板Nick名を取得する
 * 
 */
QString GikoNaviBrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return ("");
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    
    return (dataList.at(0));  
}

/**
 *  未読位置の取得
 * 
 */
int  GikoNaviBrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return (1);
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    int point = (dataList.at(2)).toInt();
    return (point);
}

/**
 *   subject.txtのファイル名(full path)を取得する
 * 
 */
QString GikoNaviBrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString datName = mLogPath + "/2ch/" + arNick + "subject.txt";
    return (datName);
}

/**
 *  最終更新時間の取得
 * 
 */
qint64 GikoNaviBrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return (0);
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    int point = (dataList.at(3)).toInt();
    return (point);
}

/**
 *   datファイルを受信する
 * 
 */
bool GikoNaviBrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(entry File)", arNick + " | " + arFile, QMessageBox::Ok);

    // ファイルを移動させる
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, false);

    // 管理情報を更新する
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/**
 *   datファイルをコピーする
 * 
 */
bool GikoNaviBrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arNick + " | " + arFile + " - ", QMessageBox::Ok);

    // ファイルをコピーする
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, false);

    // 管理情報を更新する
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/**
 *   管理情報を更新する
 * 
 */
bool GikoNaviBrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // ファイル情報を取得する
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);

    QString boardNick = arNick;
    boardNick.replace("/", "");
    GikoNaviIdxParser *parser = 0;
    int index = -1;
    int limit = mAcquiredParser.size();

    for (int loop = 0; loop < limit; loop++)
    {
        GikoNaviIdxParser *checkParser = mAcquiredParser.at(loop);
        if (checkParser != 0)
        {
            QString myNick = checkParser->getMyNick();
            myNick.replace("/", "");
            if (myNick == boardNick)
            {
                index = checkParser->exists(arFile);
                parser = checkParser;
                if (index >= 0)
                {
                    // 登録済み...更新する
                    checkParser->updateReadPoint(index, aReadPoint, nofMsg, aLastUpdate);
                }
                break;
            }   
        }
    }

    if (index < 0)
    {
        if (parser == 0)
        {
            // Folder.idxを追加する
            QString indexPath = getReceiveDatFileName(arNick, arFile);
            indexPath.replace(arFile, "Folder.idx");
            parser = new GikoNaviIdxParser(boardNick);
            parser->parseFile(indexPath, mDatFileList);
            mAcquiredParser.append(parser);
        }

        // アイテムを追加登録
        if (parser != 0)
        {
            parser->appendData(arFile, title, nofMsg, aReadPoint, aLastUpdate);
        }
    }
    return (true);
}

/**
 *   あとしまつを実行する (Folder.idx を出力する...)
 * 
 */
bool GikoNaviBrowserParser::finalize(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = 0; loop < limit; loop++)
   {
        mAcquiredParser[loop]->outputFile();
   }
   return (true);
}

/**
 *   GikoNaviIdxParserをクリアする
 * 
 */
void GikoNaviBrowserParser::clearAcquiredParser(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = limit; loop > 0; loop--)
   {
        delete mAcquiredParser[loop];
   }    
   mAcquiredParser.clear();
   mDatFileList.clear();
   return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2007- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
