/*!
 *   a2BCdatFileChecker
 * 
 *    [説明]
 * 
 * 
 */
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include "a2BCdatFileChecker.h"
#include "a2BCsubjectParser.h"
#include "datFileUtil.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCdatFileChecker::a2BCdatFileChecker(QWidget *parent)
 : QWidget(parent)
{
    resetData();
}

/*!
 *  デストラクタ
 * 
 */
a2BCdatFileChecker::~a2BCdatFileChecker()
{
    resetData();
}


/*!
 *  内部格納データのリセット
 * 
 * 
 */
void a2BCdatFileChecker::resetData(void)
{
    mDatFileTitle.clear();
    mDatFileName.clear();
    mPath.clear();    
    return;
}

/*!
 *  データファイル名を記憶する
 * 
 */
bool a2BCdatFileChecker::appendDataFile(const QString &arPath, const QString &arDatFileName)
{
    datFileUtil checker;
    if (arDatFileName.contains(".bak") == true)
    {
        // バックアップファイルだった、、、登録しない。
        return (false);
    }

    // ファイルに格納されているスレタイトルを抽出する
    QString title = checker.pickupDatFileTitle(arPath, arDatFileName);

    mDatFileTitle.append(title);
    mDatFileName.append(arDatFileName);
    mPath.append(arPath);

    return (true);
}

/*!
 *   a2b.idxへデータを出力する
 * 
 *   ※ あまりにも安直...要変更
 * 
 */
void a2BCdatFileChecker::outputDataFile(QString &arPath, QTextStream *apStream)
{
    int limit = mDatFileName.size();
    for (int index = 0; index < limit; index++)
    {
        if (mPath[index] == arPath)
        {
            (*apStream) << mDatFileName[index] << ",0,1,6,0,0," << mDatFileTitle[index] << endl;
        }
    }
    return;
}

/*!
 *   subject.txt管理ファイルに追加する...
 * 
 *   ※ あまりにも安直...要変更
 * 
 */
void a2BCdatFileChecker::appendSubjectTxtFile(QString &arPath, a2BCsubjectParser *apParser)
{
    if (apParser->isPrepare() != true)
    {
        return;
    }

    int limit = mDatFileName.size();
    for (int index = 0; index < limit; index++)
    {
        if (mPath[index] == arPath)
        {
            if (apParser->exist(mDatFileName[index]) == false)
            {
                //
                apParser->appendData(mDatFileName[index], mDatFileTitle[index]);
            }
        }
    }
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
