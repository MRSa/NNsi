/**
 *   datFileUtil
 * 
 *    [説明]
 * 
 * 
 */
#include <QFile>
#include <QIODevice>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>
#include "datFileUtil.h"

/**
 *  コンストラクタ
 * 
 */
datFileUtil::datFileUtil()
{
    //
}

/**
 *  デストラクタ
 * 
 */
datFileUtil::~datFileUtil()
{
    //
}

/**
 *  datファイルからスレタイトルを取り出す
 *   (datファイルの１行目の "<>"以降にファイル名が格納されている、その部分を抽出する)
 * 
 */
QString datFileUtil::pickupDatFileTitle(const QString &arPath, const QString &arFileName)
{
    QFile datFile(arPath + "/" + arFileName);
    if (!datFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // ファイルをオープンすることができなかった...終了する
        return (arFileName);
    }

    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    if (codec == 0)
    {
        // codecが取得できなかった...終了する
        return (arFileName);
    }

    QTextStream in(&datFile);
    in.setCodec(codec);
    QString line = in.readLine();
    datFile.close();
    if (line.isEmpty())
    {
        // 1行目を読み出すことができなかった...終了する
        return (arFileName);
    }

    int     pos = line.lastIndexOf("<>");
    if (pos < 0)
    {
        // ファイルフォーマットが違う...？ タイトルの取得ができなかったので終了する
        return (arFileName);
    }
    return (line.mid((pos + 2)));
}

/**
 *  datファイルに含まれているレス数を応答する (datファイルの行数を数えるだけ...)
 * 
 */
int datFileUtil::checkResCount(const QString &arPath, const QString &arFileName)
{
    QFile datFile(arPath + "/" + arFileName);
    if (!datFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // ファイルをオープンすることができなかった...終了する
        return (0);
    }

    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    if (codec == 0)
    {
        // codecが取得できなかった...終了する
        return (0);
    }

    QTextStream in(&datFile);
    in.setCodec(codec);
    int count = 0;
    while (1)
    {
        QString line = in.readLine();
        if (line.isNull() == true)
        {
            break;
        }
        count++;
    }
    datFile.close();
    return (count);
}

/**
 *  datファイルの移動を実行する
 * 
 * 
 */
bool datFileUtil::moveDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst)
{
    bool ret = false;
    if (isSameSrc == isSameDst)
    {
        // ファイルを移動させる
        ret = QFile::rename(arSrcFile, arDstFile);
/**
        if (ret == false)
        {
            QMessageBox::critical(0, "Error(move File)", arSrcFile + " -> " + arDstFile + " ", QMessageBox::Ok, QMessageBox::Cancel);
        }
**/
    }
    else
    {
        // ファイルを移動する
        ret = copyFile(arSrcFile, isSameSrc, arDstFile, isSameDst, true);
    }
    return (ret);
}

/**
 *  datファイルのコピーを実行する
 * 
 */
bool datFileUtil::copyDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst)
{
    bool ret = false;
    if (isSameSrc == isSameDst)
    {
        // ファイルをコピーする
        ret = QFile::copy(arSrcFile, arDstFile);
/**
        if (ret == false)
        {
            QMessageBox::critical(0, "Error(copy File)", arSrcFile + " -> " + arDstFile + " ", QMessageBox::Ok, QMessageBox::Cancel);
        }
**/
    }
    else
    {
        // ファイルをコピー
        ret = copyFile(arSrcFile, isSameSrc, arDstFile, isSameDst, false);
    }
    return (ret);
}

/**
 * 
 * 
 */
bool datFileUtil::copyFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst, bool isRemoveSrcFile)
{

    // 読み込むファイルの準備...
    QFile inData(arSrcFile);
    bool ret = inData.open(QIODevice::ReadOnly | QIODevice::Text);
//    bool ret = inData.open(QIODevice::ReadOnly);
    if (ret == false)
    {
        return (false);
    }

    qint64 fileSize = inData.size();
    char *data = new char[fileSize + 2];

    // 書き出すファイルの準備...
    if (QFile::exists(arDstFile) == true)
    {
        // コピー先ファイルがあった場合は削除
        QFile::remove(arDstFile);
    }
    QFile outData(arDstFile);
    ret = outData.open(QFile::WriteOnly | QFile::Truncate);
    if (ret == false)
    {
        inData.close();
        delete[] data;
        return (false);
    }

    qint64 readSize = inData.readLine(data, fileSize);
    while (readSize > 0)
    {
        if (data[readSize -1] == '\n')
        {
            data[readSize - 1] = '\0';
            readSize--;
        }
        if (data[readSize - 1] == '\r')
        {
            data[readSize - 1] = '\0';
            readSize--;
        }
       
        if (isSameDst == true)
        {
            readSize++;
            data[readSize - 1] = '\n';
        }
        else
        {
            readSize++;
            data[readSize - 1] = '\r';
            readSize++;
            data[readSize - 1] = '\n';
        }
        outData.write(data, readSize);
        readSize = inData.readLine(data, fileSize);        
    }

    /// 後処理...
    outData.close();
    inData.close();

    if (isRemoveSrcFile == true)
    {
        // コピー先ファイルがあった場合は削除
        QFile::remove(arDstFile);
    }
    delete[] data;
    return (true);
}

/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
