/*!
 *   a2BCbbsTableTrim
 * 
 *   説明：
 *       a2Bが使うbbstable.htmlに「a2BC」カテゴリ、板URLを追加する
 * 
 *     ※ a2BCカテゴリに追加する板URLは、45を最大とし、それを超える場合には、
 *       次(a2BC-XX、XXをインクリメントする)のカテゴリをさらに追加する。
 *       (分割の境界値は、maxBbsDatasInCategoryで設定する。)
 * 
 *     ※ 本クラスで加工した行の末尾には、"<!--a2BC-->" という文字列を追加する。
 * 
 */
#include <QList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTextCodecPlugin>
#include <QMessageBox>
#include "a2BCbbsTableTrim.h"
#include "a2BcConstants.h"
#include "a2BCfavoriteBbsData.h"
#include "bbsTableParser.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCbbsTableTrim::a2BCbbsTableTrim(QWidget *parent) : QWidget(parent)
{
    // 特に何もしない
}

/*!
 *  デストラクタ
 * 
 */
a2BCbbsTableTrim::~a2BCbbsTableTrim()
{
    // 特に何もしない
}

/*!
 *  データのリセット
 * 
 */
void a2BCbbsTableTrim::resetTrimmer()
{
    // 追加するデータを消す
    mBbsTableAppend.clear();
}

/*!
 *  板一覧データの加工準備
 *    (true : 準備OK、 false : 準備失敗)
 */
bool a2BCbbsTableTrim::prepareTrimmer(QString &arFileName)
{
    mBbsTableFileName = "";
    if (QFile::exists(arFileName) != true)
    {
        // ファイルが存在しない場合、エラー応答する
        return (false);
    }

    mBbsTableFileName = arFileName;

    QFile bbsTableFile(arFileName);
    if (!bbsTableFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // ファイルのオープン失敗、、、終了する
        return (false);
    }

    // 加工前ファイルが既にあった場合削除する
    if (QFile::exists(arFileName + ".back") == true)
    {
        if (QFile::remove(arFileName + ".back") == false)
        {
            // 加工前ファイル消去失敗...終了する
            bbsTableFile.close();
            return (false);
        }
    }

    // 加工前ファイルのオープン
    QFile bbsTableBakFile(arFileName + ".back");
    if (!bbsTableBakFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        bbsTableFile.close();
        return (false);
    }

    // 追加されたデータをクリアする
    clearLogBoardData();

    // a2BCが加工した行を抜いた行を加工前ファイルに出力する
    QTextStream in(&bbsTableFile);
    QTextStream out(&bbsTableBakFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<!--a2BC-->") != true)
        {
            // データを出力する
            out << line << endl;
        }
        else
        {
            if (line.contains("http://") == true)
            {
                // データ行を記憶する
                appendBoardUrlData(line);
            }
        }
    }
    bbsTableBakFile.close();
    bbsTableFile.close();

    return (true);
}

void a2BCbbsTableTrim::clearLogBoardData()
{
    mItemModel.clear();

    int limit = mFavoriteBbsData.size();
    for (int loop = limit - 1; loop >= 0; loop--)
    {
        delete mFavoriteBbsData[loop];
    }
    mFavoriteBbsData.clear();
}

void a2BCbbsTableTrim::appendBoardUrlData(const QString &lineData)
{
    QString boardName(lineData.section('>',1,1));
    QString boardUrl(lineData.section('>',0,0));
    QString sortKey = "";

    boardName.replace(QRegExp("</[Aa]"),"");
    boardUrl.replace(QRegExp("< *[Aa] +[Hh][Rr][Ee][Ff]="),"");

    // データを格納する
    entryBbsData(boardName, boardUrl, sortKey);

}

/*!
 *  データを１件格納する
 * 
 */
void a2BCbbsTableTrim::entryBbsData(QString &boardName, QString &boardUrl, QString &sortKey)
{
    a2BCfavoriteBbsData *bbsData = new a2BCfavoriteBbsData();
    bbsData->setData(boardName, boardUrl, sortKey);    
    mFavoriteBbsData.append(bbsData);

    // データを入れる
    QList<QStandardItem *> itemList;
    itemList << bbsData->getName() << bbsData->getUrl() << bbsData->getSortKey();
    mItemModel.appendRow(itemList);
}

/*!
 *  bbstable.htmlの加工処理メイン
 * 
 */
bool a2BCbbsTableTrim::outputTrimmer(void)
{
    // 加工前ファイルのオープン
    QFile bbsTableBakFile(mBbsTableFileName + ".back");
    if (!bbsTableBakFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }

    // 出力ファイルのオープン
    QFile bbsTableFile(mBbsTableFileName);
    if (!bbsTableFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        bbsTableBakFile.close();
        return (false);
    }
 
    // テキストストリームの準備(テキストコーデックの設定も行う)
    QTextStream out(&bbsTableFile);
    QTextStream in(&bbsTableBakFile);
    QTextCodec *codec = setCodec(out);

    // a2BCカテゴリデータのファイル出力
    outputA2BCcategory(out, codec);

    // 加工前データファイルの内容コピー
    out.setCodec(QTextCodec::codecForLocale());
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<!--a2BC-->") != true)
        {
            // 読み込んだものをそのままデータ出力する
            out << line << endl;
        }
    }
    bbsTableBakFile.close();
    bbsTableFile.close();
    
    // 加工前ファイルの削除
    QFile::remove(mBbsTableFileName + ".back");
    return (true);
}

/*!
 *  データの追加
 * 
 */
void a2BCbbsTableTrim::appendTrimmer(QByteArray &arData)
{
    mBbsTableAppend.append(arData);
    return;
}

/*!
 *   codecの準備...
 * 
 */
QTextCodec *a2BCbbsTableTrim::setCodec(QTextStream &arOutputStream)
{
    // codecクラスをつかまえる...
    QTextCodec  *codec = QTextCodec::codecForName("Shift_JIS");
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("MS_Kanji");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("sjis");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("SJIS");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("System");
    }

    // 出力streamにコーデックを設定する。
    if (codec == 0)
    {
        arOutputStream.setCodec(QTextCodec::codecForLocale());
        return (QTextCodec::codecForLocale());
    }
    arOutputStream.setCodec(codec);
    return (codec);
}

/*!
 *   a2BCカテゴリの出力
 * 
 * 
 */
void a2BCbbsTableTrim::outputA2BCcategory(QTextStream &arOutputStream, QTextCodec *codec)
{
    // codec == 0のときには、a2Bカテゴリは出力しないようにする
    if (codec == 0)
    {
        outputA2Bcategory_noCodec(arOutputStream);
        return;
    }

    // 1つのa2BCカテゴリに入れる板数の最大値 (この値を超えると分割する)
    int maxBbsDatasInCategory = 45;

    // 前処理...
    arOutputStream << "<!--a2BC-->" << endl;

    // カテゴリデータ出力のメイン
    int count = 0;
    for (QVector<QByteArray>::ConstIterator it = mBbsTableAppend.begin(); it != mBbsTableAppend.end(); it++)
    {
        ///// ここでカテゴリ名 (【a2B-xx】) を出力する
        if ((count % maxBbsDatasInCategory) == 0)
        {
            arOutputStream << codec->toUnicode("【<B>a2B");

            // "-xx"部分の出力(必要な場合には...
            if ((count / maxBbsDatasInCategory) != 0)
            {
                QString num;
                num.setNum(count / maxBbsDatasInCategory);
                arOutputStream << "-" << num;
            }
            arOutputStream << codec->toUnicode("</B>】") << "<!--a2BC-->" << endl;
        }

        // 板名(とURL)を1件分出力する
        arOutputStream << "<A HREF=" << codec->toUnicode(*it) << "</A>" << "<!--a2BC-->" << endl;
        count++;
    }

    // 後処理...
    arOutputStream << codec->toUnicode("【<B>-----</B>】") << "<!--a2BC-->" << endl;
    arOutputStream << "<!--a2BC-->" << endl;
    
    return;
}

/*!
 *   a2BCカテゴリの出力(コーデック未使用版)
 * 
 * 
 */
void a2BCbbsTableTrim::outputA2Bcategory_noCodec(QTextStream &arOutputStream)
{
    // 1つのa2BCカテゴリに入れる板数の最大値 (この値を超えると分割する)
    int maxBbsDatasInCategory = 45;

    // 前処理...
    arOutputStream << "<!--a2BC--><!--(NO CODEC)-->" << endl;

    // カテゴリデータ出力のメイン
    int count = 0;
    for (QVector<QByteArray>::ConstIterator it = mBbsTableAppend.begin(); it != mBbsTableAppend.end(); it++)
    {
        ///// ここでカテゴリ名 (【a2B-xx】) を出力する
        if ((count % maxBbsDatasInCategory) == 0)
        {
            arOutputStream << "【<B>a2B";

            // "-xx"部分の出力(必要な場合には...
            if ((count / maxBbsDatasInCategory) != 0)
            {
                QString num;
                num.setNum(count / maxBbsDatasInCategory);
                arOutputStream << "-" << num;
            }
            arOutputStream << "</B>】" << "<!--a2BC-->" << endl;
        }

        // 板名(とURL)を1件分出力する
        arOutputStream << "<A HREF=" << (*it) << "</A>" << "<!--a2BC-->" << endl;
        count++;
    }

    // 後処理...
    arOutputStream << "【<B>-----</B>】" << "<!--a2BC-->" << endl;
    arOutputStream << "<!--a2BC--><!--(NO CODEC)-->" << endl;

    return;
}

/**
 *   a2Bのログディレクトリ部分を認識する
 * 
 * 
 */
bool a2BCbbsTableTrim::readA2BLogBoards(const QString &arFileName)
{
    bbsTableParser bbsTable;
    QString fileName = arFileName;

    // 板一覧データの読み込み...
    bool readyBbs = bbsTable.prepare(fileName);
    if (readyBbs == false)
    {
        // 板一覧の解析失敗...終了する
        return (false);
    }

    // 検索用ディレクトリ名を確定する
    QString baseDirectory = fileName;
    baseDirectory.replace(QRegExp("bbstable.html$"), "");

    // ログ格納ディレクトリの一覧を取得する
    QDir baseDir(baseDirectory);
    QFileInfoList list = baseDir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir() == true)
        {
            QString dirName = fileInfo.fileName();
            if (dirName.startsWith(".") != true)
            {
                // ログディレクトリを検出、、、
                QString    url  = bbsTable.getUrl(dirName);
                QByteArray data = bbsTable.getData(dirName);

                // codecクラスをつかまえる...
                QTextCodec  *codec = QTextCodec::codecForName("Shift_JIS");
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("MS_Kanji");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("sjis");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("SJIS");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("System");
                }
                if (url.isEmpty() != true)
                {
                    QString lineData;
                    if (codec != 0)
                    {
                        lineData = "<A HREF=" + codec->toUnicode(data) + "</A>";                        
                    }
                    else
                    {
                        lineData = "<A HREF=" + data + "</A>";
                    }
                    appendBoardUrlData(lineData);
                }
            }
        }
    }    
    return (true);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
