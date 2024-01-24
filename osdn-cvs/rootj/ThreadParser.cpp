/*****************************************************************************
  $Id: ThreadParser.cpp,v 1.1 2005/04/24 13:20:16 mrsa Exp $
       スレ一覧を解析するクラス
 *****************************************************************************/
#include "qiodevice.h"
#include "qsjiscodec.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "qtable.h"
#include "ThreadParser.h"

namespace rootj {

// コンストラクタ
ThreadParser::ThreadParser() : mpProgress(0)
{
    // 今のところ何もしない
}

// デストラクタ
ThreadParser::~ThreadParser()
{
    // 今のところ何もしない
}

// クラスの初期化
void ThreadParser::initializeSelf(QProgressBar *apProgress)
{
    mpProgress = apProgress;
}

// 板一覧の解析を実行する
bool ThreadParser::parse(QIODevice *apSourceData, QTable *apRoot)
{
    //QMessageBox::information(NULL,QObject::tr("information"),QObject::tr("ThreadParser::parse()"),QMessageBox::Ok);

    bool result = false;
    if ((apSourceData == 0)||(apRoot == 0))
    {
        // 引数の指定が正しくない、終了する
        return (result);
    }

    // (本来ないはずだが)データソースがオープンされていれば、いったんclose
    if (apSourceData->isOpen() == true)
    {
        apSourceData->close();
    }

    // データソースをオープンする
    apSourceData->open(IO_ReadOnly);

    // 先頭に...
    apSourceData->reset();

    // プログレスバーを(使用可能に)初期化する
    if (mpProgress != 0)
    {
        mpProgress->setEnabled(true);
        mpProgress->reset();
    }
    int bufSize = apSourceData->size();
    mpProgress->setTotalSteps(bufSize);
    mpProgress->setProgress(0);

    // ... ここでスレ一覧の解析実処理 ...
    result = parseMain(apSourceData, apRoot);

    // データソースをクローズする
    apSourceData->close();

    // プログレスバーを(使い終わったので)使用不可にする
    if (mpProgress != 0)
    {
        mpProgress->reset();
        mpProgress->setEnabled(false);
    }
    return (result);
}

// 解析処理のメイン部分
bool ThreadParser::parseMain(QIODevice *apSourceData, QTable *apRoot)
{
    // データサイズを取得する
    int dataSize = apSourceData->size();

    // バッファを確保する。。。
    char *temporaryBuffer = static_cast<char *> (malloc(dataSize + 100));
    if (temporaryBuffer == NULL)
    {
        return (false);
    }
    memset(temporaryBuffer, 0x00, (dataSize + 100));

    // データを一気に読み込む
    int readSize = apSourceData->readBlock(temporaryBuffer, (dataSize + 100));
    if (readSize < 0)
    {
        free(temporaryBuffer);
        return (false);
    }
    // データの末尾を見つける
    char *topPointer    = temporaryBuffer;
    char *bottomPointer = topPointer;
    int   row           = 0;

    while (bottomPointer < (temporaryBuffer + dataSize))
    {
        bottomPointer = strstr(topPointer, "\x0a");
        if (bottomPointer == NULL)
        {
            // データがなくなった、抜ける
            break;
	}
        // 一行追加する...
        apRoot->insertRows(row);

        // スレ情報を解析する。
        parseThreadInfo(topPointer, (bottomPointer - topPointer), apRoot, row);

        // 次の項目へすすめる。
        topPointer = bottomPointer + 1;
        row++;

        // プログレスバーを更新する
        mpProgress->setProgress(topPointer - temporaryBuffer);
    }

    // 確保したバッファを開放して終了する
    free(temporaryBuffer);

    return (true);
}

// １つのカテゴリのデータを取得
void ThreadParser::parseThreadInfo(char       *apBuffer,
                                   int         aSize, 
                                   QTable     *apTable, 
                                   int         aRow)
{
    QSjisCodec  codec;

    int  size;
    char buffer[480];
    memset (buffer, 0x00, sizeof(buffer));
    size = aSize;
    if (size > (sizeof(buffer) - 1))
    {
        size = (sizeof(buffer) - 1);
    }
    memmove(buffer, apBuffer, size);

    char *ptr = strstr(buffer, "<>");
    if (ptr != NULL)
    {
        QString threadNumber = codec.toUnicode(buffer, (ptr - buffer));
        apTable->setText(aRow, 2, threadNumber);
        ptr = ptr + sizeof("<>") - 1;
    }
    else
    {
        ptr = buffer + size;
    }

    char *ptr2 = buffer + size;
    while (ptr2 != ptr)
    {
        ptr2--;
        if (*ptr2 == '(')
        {
            break;
        }
    }
    if (ptr == ptr2)
    {
        ptr2 = buffer + size;
    }

    QString threadTitle = "?????";
    if (ptr != ptr2)
    {
        threadTitle = codec.toUnicode(ptr, (ptr2 - ptr));
    }
    apTable->setText(aRow, 0, threadTitle);

    int number = 0;
    if (*ptr2 == '(')
    {
        ptr2++;
        while ((*ptr2 >= '0')&&(*ptr2 <= '9'))
        {
            number = number * 10;
            number = number + (*ptr2 - '0');
            ptr2++;
        }
    }
    QString nofThread;
    nofThread.setNum(number);
    apTable->setText(aRow, 1, nofThread);

    //QMessageBox::information(NULL, "Thread Info.", threadTitle,QMessageBox::Ok);
    return;
}

}  // namespace rootj {


