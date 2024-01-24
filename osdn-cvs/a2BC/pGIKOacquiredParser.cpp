#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include "pGIKOacquiredParser.h"
#include "a2BcConstants.h"

/*!
 *  コンストラクタ
 * 
 */
pGIKOacquiredParser::pGIKOacquiredParser(QWidget *parent) : QWidget(parent)
{

}

/*!
 *  デストラクタ
 * 
 */
pGIKOacquiredParser::~pGIKOacquiredParser()
{

}

/*!
 *  ファイルを解析する
 * 
 * 
 */
int pGIKOacquiredParser::parseFile(const QString &arFileName)
{
    // ファイル名を記憶する
    mFileName = arFileName;

    int count = 0;
    if (QFile::exists(arFileName) != true)
    {
        // ファイルが存在しない場合、何もしない
        return (count);
    }

    QFile indexFile(arFileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (count);
    }
    
    QTextStream in(&indexFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (process_line(arFileName, line) == true)
        {
            count++;
        }
    }
    indexFile.close();
    
    return (count);
}

/*!
 *   acquired.txt の１行を解析する
 * 
 */
bool pGIKOacquiredParser::process_line(const QString &arFileName, QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        return (false);
    }
 
    QString datFileName = arLineData.section("<>", 0, 0);
    QString datTitle    = arLineData.section("<>", 1, 1);
    QString curNumStr   = arLineData.section("<>", 2);
    
    int pos = datTitle.lastIndexOf(" (");
    QString maxNumStr = datTitle.mid(pos);
    maxNumStr.replace("(", "");
    maxNumStr.replace(" ", "");
    maxNumStr.replace(")", "");
   
    if (datFileName.contains(".dat") == false)
    {
        return (false);
    }
    QString title = datTitle.left(pos);
    return (appendData(datFileName, title, maxNumStr.toInt(), curNumStr.toInt()));
}

/*!
 *   acquired.txt を出力する
 * 
 */
void pGIKOacquiredParser::outputFile(void)
{
//  QMessageBox::information(0, "Info(output file)", mFileName, QMessageBox::Ok);

    QTextStream *out = 0;

    // 古いファイルがあった場合、リネームする
    if (QFile::exists(mFileName) == true)
    {
        QFile::remove(mFileName + ".bak");
        QFile::rename(mFileName, mFileName + ".bak");
    }
    QFile indexFile(mFileName);
    if (!indexFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    out = new QTextStream(&indexFile);
    if (out == 0)
    {
        return;
    }

    int limit = mDatFileName.size();
    for (int index = 0; index < limit; index++)
    {
        *out << mDatFileName[index] << "<>" << mTitle[index] << " (" << mNofMessage[index] << ")<>" << mCurrentMessage[index] << endl;
    }
    if (out != 0)
    {
        out->flush();
        delete out;
        out = 0;
    }
    clearFile();
    QFile::remove(mFileName + ".bak");
    return;
}

/*!
 *   内部データを初期化する
 * 
 */
void pGIKOacquiredParser::clearFile()
{
    mDatFileName.clear();
    mTitle.clear();
    mNofMessage.clear();
    mCurrentMessage.clear();
    return;
}

/*!
 * 
 * 
 * 
 */
int pGIKOacquiredParser::exists(const QString &arFileName)
{
    for (int lp = 0; lp < mDatFileName.size(); lp++)
    {
        if (mDatFileName[lp] == arFileName)
        {
            return (lp);
        }
    }
    return (-1);
}

int pGIKOacquiredParser::getNofMessage(int index)
{
    if ((index < 0)||(index >= mDatFileName.size()))
    {
        return (1);
    }
    return (mNofMessage[index]);
}

int pGIKOacquiredParser::getCurrentMessage(int index)
{
    if ((index < 0)||(index >= mDatFileName.size()))
    {
        return (1);
    }
    return (mCurrentMessage[index]);
}

QString pGIKOacquiredParser::getTitle(int index)
{
    if ((index < 0)||(index >= mDatFileName.size()))
    {
        return ("");
    }
    return (mTitle[index]);
}

void pGIKOacquiredParser::setNofMessage(int index, int status)
{
    if ((index < 0)||(index >= mDatFileName.size()))
    {
        return;
    }
    mNofMessage[index] = status;
    
    return;    
}

void pGIKOacquiredParser::setCurrentMessage(int index, int status)
{
    if ((index < 0)||(index >= mDatFileName.size()))
    {
        return;
    }
    mCurrentMessage[index] = status;

    return;    
}

void pGIKOacquiredParser::setTitle(int index, QString &arTitleName)
{
    if ((index < 0)||(index >= mDatFileName.size()))
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
bool pGIKOacquiredParser::appendData(QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage)
{
    mDatFileName.append(arDatFileName);
    mTitle.append(arTitle);
    mNofMessage.append(aNofMessage);
    mCurrentMessage.append(aCurrentMessage);
    
    return (true);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
