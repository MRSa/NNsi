#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include "janeIndexParser.h"
#include "a2BcConstants.h"

const int process_limit_line = 15;

/*!
 *  コンストラクタ
 * 
 */
janeIndexParser::janeIndexParser()
{
    // 何もしない...
}

/*!
 *  デストラクタ
 * 
 */
janeIndexParser::~janeIndexParser()
{
    // 何もしない...
}

/**
 * 
 *  内部情報をクリアする
 * 
 */
void janeIndexParser::prepare(void)
{
    clearVariables();
    return;
}

/*!
 *  ファイルを解析する
 * 
 * 
 */
bool janeIndexParser::parseFile(const QString &arPath, const QString &arIdxFileName)
{
    QString fileName = arPath + "/" + arIdxFileName;
//    QMessageBox::information(0, "Info(parseFile)",  fileName, QMessageBox::Ok);

    if (QFile::exists(fileName) != true)
    {
        // ファイルが存在しない場合、何もしない
        return (false);
    }

    QFile indexFile(fileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }

    // ファイル名とパスを記憶する
    QString path = arPath;
    process_line(-2, path);
    QString name = arIdxFileName;
    process_line(-1, name);
    
    int count = 0;
    QTextStream in(&indexFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        process_line(count, line);
        count++;
    }
    for (;count < process_limit_line; count++)
    {
        // あまりの領域（とダミーデータ）を更新する
        QString line = "";
        process_line(count, line);
    }
    indexFile.close();
    
    return (true);
}

/*!
 *   インデックスファイルの１行を解析する
 * 
 */
void janeIndexParser::process_line(const int count, QString &arLineData)
{
    switch (count)
    {
          case -2:
            // ファイルパスを記録する
            mFilePath.append(arLineData);
            break;

          case -1:
            // Idxファイル名を記録する
            mIdxFileName.append(arLineData);
            break;

          case 0:
            // スレタイトルを格納する
            mTitle.append(arLineData);
            break;

          case 1:
            // 最終更新日を格納する
            mLastUpdate.append(arLineData);
            break;

          case 2:
            // レス格納数を格納する
            mNofMsg.append(arLineData);
            break;

          case 3:
            // 現在のレス番号を格納する
            mCurMsg.append(arLineData);
            break;

          case 4:
            // ダミーデータ（？）を格納する
            mDummy1.append(arLineData);
            break;

          case 5:
            // URLを格納する
            mUrl.append(arLineData);
            decideBoardNick(arLineData);
            break;

          case 6:
            // ダミーデータ（０）を格納する
            mDummy2.append(arLineData);
            break;

          case 7:
            // ダミーデータ（０）を格納する
            mDummy3.append(arLineData);
            break;

          case 8:
            // ダミーデータ（？）を格納する
            mDummy4.append(arLineData);
            break;

          case 9:
            // ダミーデータ（？）を格納する
            mDummy5.append(arLineData);
            break;

          case 10:
            // ダミーデータ（０）を格納する
            mDummy6.append(arLineData);
            break;

          case 11:
            // ？？？を格納する
            mUpdate.append(arLineData);
            break;

          case 12:
            // ダミーデータ（０）を格納する
            mDummy7.append(arLineData);
            break;

          case 13:
            // ダミーデータ（？）を格納する
            mDummy8.append(arLineData);
            break;

          case 14:
            // ダミーデータ（？）を格納する
            mDummy9.append(arLineData);
            break;

          default:
            // 
            break;
    }
    return;
}

/*!
 *   板Nickを抽出する
 * 
 */
void janeIndexParser::decideBoardNick(QString &arUrl)
{
    int pos = arUrl.lastIndexOf("/");
    
    QString nick = arUrl.mid((pos + 1)) + "/";
    mNick.append(nick);

    return;
}


/*!
 *   インデックスファイルを出力する
 * 
 */
void janeIndexParser::outputFile(void)
{
    int limit = mIdxFileName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        outputFileMain(loop);
    }
    clearVariables();
    return;
}

/*!
 *   インデックスファイルを出力する
 * 
 */
void janeIndexParser::outputFileMain(int index)
{
//  QMessageBox::information(0, "Info(output file)", mIdxFileName[index], QMessageBox::Ok);

    QTextStream *out = 0;
    QString fileName = mFilePath[index] + "/" + mIdxFileName[index];

    // 古いファイルがあった場合、リネームする
    if (QFile::exists(fileName) == true)
    {
        QFile::remove(fileName + ".bak");
        QFile::rename(fileName, fileName + ".bak");
    }
    QFile indexFile(fileName);
    if (!indexFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    out = new QTextStream(&indexFile);
    if (out == 0)
    {
        return;
    }

    *out << mTitle[index] << endl;
    *out << mLastUpdate[index] << endl;
    *out << mNofMsg[index] << endl;
    *out << mCurMsg[index] << endl;
    *out << mDummy1[index] << endl;
    *out << mUrl[index] << endl;
    *out << mDummy2[index] << endl;
    *out << mDummy3[index] << endl;
    *out << mDummy4[index] << endl;
    *out << mDummy5[index] << endl;
    *out << mDummy6[index] << endl;
    *out << mUpdate[index] << endl;
    *out << mDummy7[index] << endl;
    *out << mDummy8[index] << endl;
    *out << mDummy9[index] << endl;

    out->flush();
    delete out;
    out = 0;
    QFile::remove(fileName + ".bak");
    return;
}

/*!
 *   内部データを初期化する
 * 
 */
void janeIndexParser::clearVariables(void)
{
    mIdxFileName.clear();
    mTitle.clear();
    mLastUpdate.clear();
    mNofMsg.clear();
    mCurMsg.clear();
    mDummy1.clear();
    mUrl.clear();
    mDummy2.clear();
    mDummy3.clear();
    mDummy4.clear();
    mDummy5.clear();
    mDummy6.clear();
    mUpdate.clear();
    mDummy7.clear();
    mDummy8.clear();
    mDummy9.clear();
    mNick.clear();

    return;
}

/*!
 *  ファイルが登録されているかどうか確認する
 * 
 * 
 */
int janeIndexParser::exists(const QString &arFileName)
{
    for (int lp = 0; lp < mIdxFileName.size(); lp++)
    {
        QString target = mIdxFileName[lp];
        target.replace("idx", "dat");
        if (target == arFileName)
        {
            return (lp);
        }
    }
    return (-1);
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::count(void)
{
    return (mIdxFileName.size());
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::getNofMessage(int index)
{
    if ((index < 0)||(index >= mNofMsg.size()))
    {
        return (1);
    }
    QString nofMsg = mNofMsg[index];
    return (nofMsg.toInt());
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::getCurrentMessage(int index)
{
    if ((index < 0)||(index >= mCurMsg.size()))
    {
        return (1);
    }
    QString curMsg = mCurMsg[index];
    return (curMsg.toInt());
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getTitle(int index)
{
    if ((index < 0)||(index >= mTitle.size()))
    {
        return ("");
    }
    return (mTitle[index]);
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getDatFileName(int index)
{
    if ((index < 0)||(index >= mIdxFileName.size()))
    {
        return ("");
    }
    QString target = mIdxFileName[index];
    target.replace("idx", "dat");
    return (target);
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getNickName(int index)
{
    if ((index < 0)||(index >= mNick.size()))
    {
        return ("");
    }
    return (mNick[index]);
}

/*!
 * 
 * 
 * 
 */QString janeIndexParser::getLastUpdate(int index)
{
    if ((index < 0)||(index >= mLastUpdate.size()))
    {
        return ("");
    }
    return (mLastUpdate[index]);
}


/*!
 * 
 * 
 * 
 */
void janeIndexParser::setNofMessage(int index, int status)
{
    if ((index < 0)||(index >= mNofMsg.size()))
    {
        return;
    }
    QString num;
    num.setNum(status);
    mNofMsg[index] = num;    
    return;    
}

/*!
 * 
 * 
 * 
 */
void janeIndexParser::setCurrentMessage(int index, int status)
{
    if ((index < 0)||(index >= mCurMsg.size()))
    {
        return;
    }

    QString num;
    num.setNum(status);
    mCurMsg[index] = num;
    return;    
}

/*!
 * 
 * 
 * 
 */
void janeIndexParser::setTitle(int index, QString &arTitleName)
{
    if ((index < 0)||(index >= mTitle.size()))
    {
        return;
    }
    mTitle[index] = arTitleName;
    return;
}

/*!
 *  スレデータを登録する
 * 
 */
bool janeIndexParser::appendData(QString &arNick, QString &arUrl, QString &arFilePath, QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage)
{
    QString nofMsg, curMsg, threadNumber, datFileName;
    nofMsg.setNum(aNofMessage);
    curMsg.setNum(aCurrentMessage);
    
    threadNumber = arDatFileName;
    threadNumber.replace(".dat", "");
    datFileName = threadNumber + ".idx";
    
    mFilePath.append(arFilePath);
    mIdxFileName.append(datFileName);
    mTitle.append(arTitle);
    mLastUpdate.append("");
    mNofMsg.append(nofMsg);
    mCurMsg.append(curMsg);
    mDummy1.append("");
    mUrl.append(arUrl);
    mDummy2.append("0");
    mDummy3.append("0");
    mDummy4.append("");
    mDummy5.append("");
    mDummy6.append("0");
    mUpdate.append(threadNumber);
    mDummy7.append("0");
    mDummy8.append("");
    mDummy9.append("");
    mNick.append(arNick);
    
    return (true);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
