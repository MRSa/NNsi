#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <QTextCodec>
#include "a2BCsubjectParser.h"
#include "a2BcConstants.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCsubjectParser::a2BCsubjectParser(QWidget *parent) : QWidget(parent)
{
    mPrepare = false;
    mFileName = "";
    mSubjectData.clear();
}

/*!
 *  デストラクタ
 * 
 */
a2BCsubjectParser::~a2BCsubjectParser()
{
    mPrepare = false;
    mFileName = "";
    mSubjectData.clear();
}

/*!
 *  ファイルを解析する
 * 
 * 
 */
int a2BCsubjectParser::parseSubjectFile(const QString &arFileName)
{
    // データをまずクリアする...
    clearSubjectFile();

    int count = 0;
    if (QFile::exists(arFileName) != true)
    {
        // ファイルが存在しない場合、何もしない
        return (count);
    }
    mFileName = arFileName;

    QFile subjectFile(arFileName);
    if (!subjectFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (count);
    }
    
    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    QTextStream in(&subjectFile);
    in.setCodec(codec);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (process_line(line) == true)
        {
            count++;
        }
    }
    subjectFile.close();
    QFile::remove(mFileName + ".bak");
    QFile::rename(mFileName, mFileName + ".bak");

    mPrepare = true;
    return (count);
}

/*!
 * 
 * 
 */
bool a2BCsubjectParser::process_line(QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        return (false);
    }
    
    if (arLineData.contains(".dat") == false)
    {
        return (false);
    }
    mSubjectData.append(arLineData);
    
    return (true);
}

/*!
 *
 * 
 */
void a2BCsubjectParser::outputSubjectFile(void)
{
    if (QFile::exists(mFileName) == true)
    {
        // ファイルが存在する場合、リネームする...
        QFile::remove(mFileName + ".bak");
        QFile::rename(mFileName, mFileName + ".bak");
    }

    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    QFile subjectFile(mFileName);
    if (!subjectFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&subjectFile);
    out.setCodec(codec);

    ///// /////
    for (QVector<QString>::ConstIterator it = mSubjectData.begin(); it != mSubjectData.end(); it++)
    {
        out << (*it) << endl;
    }
    ///// /////

    out.flush();
    subjectFile.close();

    return;
}


/*!
 * 
 * 
 */
void a2BCsubjectParser::clearSubjectFile()
{
    mPrepare = false;
    mFileName = "";

    mSubjectData.clear();
    return;
}

/*!
 * 
 * 
 */
bool a2BCsubjectParser::isPrepare()
{
    return (mPrepare);    
}

/*!
 *   存在するかどうか確認する
 * 
 */
bool a2BCsubjectParser::exist(const QString &arFileName)
{
    for (QVector<QString>::ConstIterator it = mSubjectData.begin(); it != mSubjectData.end(); it++)
    {
        if ((*it).contains(arFileName, Qt::CaseInsensitive) == true)
        {
            return (true);
        }
    }
    return (false);
}

/*!
 * 
 * 
 */
void a2BCsubjectParser::appendData(QString &arFileName, QString &arTitle)
{
    QString data =  arFileName + "<>" + arTitle;
    //data.truncate(data.length() - 2);
    data = data + " (0)";
    mSubjectData.append(data);

    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
