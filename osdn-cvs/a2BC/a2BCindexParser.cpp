#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "a2BCindexParser.h"
#include "a2BcConstants.h"
#include "a2BCsubjectParser.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCindexParser::a2BCindexParser(QWidget *parent) : QWidget(parent)
{

}

/*!
 *  デストラクタ
 * 
 */
a2BCindexParser::~a2BCindexParser()
{

}

/*!
 *  ファイルを解析する
 * 
 * 
 */
int a2BCindexParser::parseIndexFile(const QString &arFileName)
{
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
 * 
 * 
 */
bool a2BCindexParser::process_line(const QString &arFileName, QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        return (false);
    }
 
    QString datFileName = arLineData.section(",", 0, 0);
    QString maxNumStr   = arLineData.section(",", 1, 1);
    QString curNumStr   = arLineData.section(",", 2, 2);
    QString statusStr   = arLineData.section(",", 3, 3);
    QString modifiedStr = arLineData.section(",", 4, 4);
    QString reserve2Str = arLineData.section(",", 5, 5);
    QString optionStr   = arLineData.section(",", 6);
    
    if (datFileName.contains(".dat") == false)
    {
        return (false);
    }

    mLastUpdate.append(modifiedStr.toULongLong());
    mReserve2.append(reserve2Str);
    mOptionData.append(optionStr);      
    mCurrentMessage.append(curNumStr.toInt());
    mNofMessage.append(maxNumStr.toInt());
    mDatFileStatus.append(statusStr.toInt());
    mDatFileName.append(datFileName);
    mIndexFileName.append(arFileName);
    
    return (true);
}

/*!
 *
 * 
 */
void a2BCindexParser::outputIndexFile(bool aRemoveError, bool aRemoveOver, bool outputSubjectTxt, a2BCdatFileChecker *apDatFileChecker)
{
    a2BCsubjectParser *subjectParser = new a2BCsubjectParser();
    QTextStream *out = 0;
    QFile *indexFile = 0;
    QString fileName = "";
    int limit = mIndexFileName.size();
    for (int index = 0; index < limit; index++)
    {
        if (mIndexFileName[index] != fileName)
        {
            if (outputSubjectTxt == true)
            {
                QString dirName = fileName;
                dirName.replace("/a2b.idx", "");   // a2b.idx を取り去る
                if (apDatFileChecker != 0)
                {
                    apDatFileChecker->outputDataFile(dirName, out);
                    apDatFileChecker->appendSubjectTxtFile(dirName, subjectParser);
                }
            }

            // ファイルオープン...
            fileName = mIndexFileName[index];
            
            if (out != 0)
            {
                *out << ";[EOF]" << endl;
                out->flush();
                delete out;
                out = 0;
            }
            
            if (indexFile != 0)
            {
                indexFile->close();
                delete indexFile;
                indexFile = 0;
            }

            // subject.txt にない場合には追記する
            if (outputSubjectTxt == true)
            {
                if (subjectParser->isPrepare() == true)
                {
                    subjectParser->outputSubjectFile();
                }
                subjectParser->clearSubjectFile();
                QString subjectTxt = fileName;
                subjectTxt.replace("a2b.idx", "subject.txt");   // a2b.idx を subject.txtに置換する
                subjectParser->parseSubjectFile(subjectTxt);
            }

            // 古いファイルがあった場合、リネームする
            if (QFile::exists(fileName) == true)
            {
                QFile::remove(fileName + ".bak");
                QFile::rename(fileName, fileName + ".bak");
            }
            indexFile = new QFile(fileName);
            if (!indexFile->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                delete indexFile;
                indexFile = 0;
                fileName = "";
                continue;
            }
            out = new QTextStream(indexFile);
            if (out == 0)
            {
                delete indexFile;
                indexFile = 0;
                fileName = "";
                continue;
            }

            if (outputSubjectTxt == true)
            {
                if (subjectParser->exist(mDatFileName[index]) == false)
                {
                    //
                    subjectParser->appendData(mDatFileName[index], mOptionData[index]);
                    mDatFileStatus[index] = A2BCSTATUS_UNKNOWN;
                }
            }
            *out << "; FileName, Max, Current, Status, lastAccess, 0, (Title)" << endl;
            if ((aRemoveError == true)&&(mDatFileStatus[index] == A2BCSTATUS_GETERROR))
            {
                // 出力しない
//                (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
            }
            else if ((aRemoveOver == true)&&(mNofMessage[index] > 1000))
            {
                // 出力しない
//                (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
            }
            else // if ((aRemoveError == false)&&(aRemoveOver == false))
            {
                (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
            }
       }
        else
        {
            if (outputSubjectTxt == true)
            {
                if (subjectParser->exist(mDatFileName[index]) == false)
                {
                    //
                    subjectParser->appendData(mDatFileName[index], mOptionData[index]);
                    mDatFileStatus[index] = A2BCSTATUS_UNKNOWN;
                }
            }            
            if (out != 0)
            {
                if ((aRemoveError == true)&&(mDatFileStatus[index] == A2BCSTATUS_GETERROR))
                {
                    // 出力しない
//                    (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
                }
                else if ((aRemoveOver == true)&&(mNofMessage[index] > 1000))
                {
                    // 出力しない
//                    (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
                }
                else // if ((aRemoveError == false)&&(aRemoveOver == false))
                {
                    (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mLastUpdate[index] << "," << mReserve2[index] << "," << mOptionData[index] << endl;
                }
             }
        }
    }
    if (out != 0)
    {
        if (outputSubjectTxt == true)
        {
            QString dirName = fileName;
            dirName.replace("/a2b.idx", "");   // a2b.idx を取り去る
            if (apDatFileChecker != 0)
            {
                apDatFileChecker->outputDataFile(dirName, out);
            }
        }
        *out << ";[EOF]" << endl;
        out->flush();
        delete out;
        out = 0;
    }

    if (outputSubjectTxt == true)
    {
        if (subjectParser->isPrepare() == true)
        {
            subjectParser->outputSubjectFile();
        }
        subjectParser->clearSubjectFile();
    }

    if (indexFile != 0)
    {
           indexFile->close();
           delete indexFile;
           indexFile = 0;
    }
    delete subjectParser;
    clearIndexFile();
    return;
}


/*!
 * 
 * 
 */
void a2BCindexParser::clearIndexFile()
{
    mIndexFileName.clear();
    mDatFileName.clear();
    mDatFileStatus.clear();
    mOptionData.clear();
    mLastUpdate.clear();
    mReserve2.clear();      
    mCurrentMessage.clear();
    mNofMessage.clear();
    return;
}

/*!
 * 
 * 
 * 
 */
QString a2BCindexParser::getIndexFileName(int index)
{
    if ((index >= mIndexFileName.size())||(index < 0))
    {
        return ("");
    }
    return (mIndexFileName[index]);
}

/*!
 * 
 * 
 * 
 */
int a2BCindexParser::exists(const QString &arFileName)
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

/*!
 * 
 * 
 * 
 */
QString a2BCindexParser::getFileName(int index)
{
    if ((index >= mDatFileName.size())||(index < 0))
    {
        return ("");
    }
    return (mDatFileName[index]);
}

/*!
 * 
 * 
 * 
 */
quint64 a2BCindexParser::getLastUpdate(int index)
{
    if ((index >= mLastUpdate.size())||(index < 0))
    {
        return (0);
    }
    return (mLastUpdate[index]);
}

/*!
 * 
 * 
 * 
 */
void a2BCindexParser::setLastUpdate(int index)
{
    if ((index >= mLastUpdate.size())||(index < 0))
    {
        return;
    }
    QDateTime datetime = QDateTime::currentDateTime();
    quint64 currentTime = (quint64) datetime.toTime_t();
    currentTime = currentTime * 1000;
    mLastUpdate[index] = currentTime;
    return;
}

/*!
 * 
 * 
 * 
 */
QString a2BCindexParser::getOptionData(int index)
{
    if ((index >= mOptionData.size())||(index < 0))
    {
        return ("");
    }
    return (mOptionData[index]);
}

/*!
 * 
 * 
 * 
 */
int a2BCindexParser::getNofMessage(int index)
{
    if ((index >= mNofMessage.size())||(index < 0))
    {
        return (0);
    }
    return (mNofMessage[index]);
}

/*!
 * 
 * 
 * 
 */
int a2BCindexParser::getCurrentMessage(int index)
{
    if ((index >= mCurrentMessage.size())||(index < 0))
    {
        return (0);
    }
    return (mCurrentMessage[index]);
}

/*!
 * 
 * 
 * 
 */
int a2BCindexParser::getFileStatus(int index)
{
    if ((index >= mDatFileStatus.size())||(index < 0))
    {
        return (0);
    }
    return (mDatFileStatus[index]);
}

/*!
 * 
 * 
 * 
 */
void a2BCindexParser::setFileStatus(int index, int status)
{
    if ((index >= mDatFileStatus.size())||(index < 0))
    {
        return;
    }
    if (status == A2BCSTATUS_UNKNOWN)
    {
        if ((mDatFileStatus[index] != A2BCSTATUS_GETERROR)&&
            (mDatFileStatus[index] != A2BCSTATUS_NOTYET)&&
            (mDatFileStatus[index] != A2BCSTATUS_RESERVE))
        {
            // エラー以外の時には状態を更新しない
            return;
        }
        status = A2BCSTATUS_NEW;
    }
    mDatFileStatus[index] = status;

    setLastUpdate(index);
    return;
}

/*!
 * 
 * 
 * 
 */
void a2BCindexParser::setNofMessage(int index, int number)
{
    if ((index >= mNofMessage.size())||(index < 0))
    {
        return;
    }
    if (number <= 0)
    {
        number = 1;
    }
    if (number > 1000)
    {
        number = 1001;
    }    
    mNofMessage[index] = number;

    return;
}

/*!
 * 
 * 
 * 
 */
void a2BCindexParser::setCurrentMessage(int index, int number)
{
    if ((index >= mCurrentMessage.size())||(index < 0))
    {
        return;
    }
    if (number <= 0)
    {
        number = 1;
    }
    if (number > 1000)
    {
        number = 1001;
    }    
    mCurrentMessage[index] = number;

    return;
}

QString a2BCindexParser::getNickName(int index)
{
    if ((index >= mIndexFileName.size())||(index < 0))
    {
        return ("");
    }
    QString indexFile = mIndexFileName[index];
    
    int pos = indexFile.lastIndexOf("/");
    QString aaa = indexFile.left(pos);
    pos = aaa.lastIndexOf("/");
    
    return (aaa.mid((pos + 1)));
}


bool a2BCindexParser::insertData(QString &arIndexFileName, QString &arDatFileName, QString &arReserve2, QString &arOption, int currentMessage, int nofMessage, qint64 lastUpdate)
{
    for (int lp = 0; lp < mIndexFileName.size(); lp++)
    {
        if (mIndexFileName[lp] == arIndexFileName)
        {
            mLastUpdate.insert    (lp, lastUpdate);
            mReserve2.insert      (lp, arReserve2);
            mOptionData.insert    (lp, arOption);
            mCurrentMessage.insert(lp, currentMessage);
            mNofMessage.insert    (lp, nofMessage);
            mDatFileStatus.insert (lp, A2BCSTATUS_UNKNOWN);
            mDatFileName.insert   (lp, arDatFileName);
            mIndexFileName.insert (lp, arIndexFileName);

            return (true);
        }
    }
    mLastUpdate.append(lastUpdate);
    mReserve2.append(arReserve2);
    mOptionData.append(arOption);      
    mCurrentMessage.append(currentMessage);
    mNofMessage.append(nofMessage);
    mDatFileStatus.append(A2BCSTATUS_UNKNOWN);
    mDatFileName.append(arDatFileName);
    mIndexFileName.append(arIndexFileName);
    
    return (false);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
