/*****************************************************************************
  $Id: BoardParser.cpp,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       板一覧を解析するクラス
 *****************************************************************************/
#include "qiodevice.h"
#include "qsjiscodec.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "qlistview.h"
#include "BoardParser.h"

namespace rootj {

// コンストラクタ
BoardParser::BoardParser() : mpProgress(0)
{
    // 今のところ何もしない
}

// デストラクタ
BoardParser::~BoardParser()
{
    // 今のところ何もしない
}

// クラスの初期化
void BoardParser::initializeSelf(QProgressBar *apProgress)
{
    mpProgress = apProgress;
}

// 板一覧の解析を実行する
bool BoardParser::parse(QIODevice *apSourceData, QString &apName, QListView *apRoot)
{
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

    // ツリーを作成する
    QListViewItem *treeNode = new QListViewItem(apRoot);
    treeNode->setText(0, apName);

    // ... ここで板一覧の解析実処理 ...
    result = parseMain(apSourceData, treeNode);

    // ルートにつなぐ
    apRoot->insertItem(treeNode);

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

// 解析処理のメイン
bool BoardParser::parseMain(QIODevice *apSourceData, QListViewItem *apRoot)
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
    int readSize = apSourceData->readBlock(temporaryBuffer, (dataSize + 100));
    if (readSize < 0)
    {
        free(temporaryBuffer);
        return (false);
    }

    // カテゴリの先頭を見つける
    char *currentPointer = strstr(temporaryBuffer, "<BR><BR><B>");
    if (currentPointer == NULL)
    {
        // カテゴリがなかった...
        free(temporaryBuffer);
        return (false);
    }
    currentPointer = currentPointer + sizeof("<BR><BR><B>") - 1;

    while (currentPointer < (temporaryBuffer + dataSize))
    {
        // 次のカテゴリの先頭を見つける
        char *nextPointer = strstr(currentPointer, "<BR><BR><B>");
        if (nextPointer == NULL)
        {
            nextPointer = temporaryBuffer + dataSize;
	}

        // カテゴリデータの解析
        QListViewItem *listItem = new QListViewItem(apRoot);
        parseCategory(currentPointer,(nextPointer - currentPointer),listItem);

        // 表示領域につなぐ
        apRoot->insertItem(listItem);

        // 次のカテゴリの先頭へ移動...
        currentPointer = nextPointer + sizeof("<BR><BR><B>") - 1;
        mpProgress->setProgress((currentPointer - temporaryBuffer));
    }

    // 確保したバッファを開放する
    free(temporaryBuffer);
    return (true);
}

// １つのカテゴリのデータを取得
void BoardParser::parseCategory(char          *apBuffer,
                                int            aSize, 
                                QListViewItem *apItemList)
{
    QSjisCodec  codec;
    QString     categoryName, bbsBuffer;

    char stringBuffer[100];
    memset (stringBuffer, 0x00, sizeof(stringBuffer));
    char *bottomPtr = strstr(apBuffer, "</B>");
    if (bottomPtr == NULL)
    {
        // カテゴリがない...(仮のカテゴリを設定する)
        categoryName = "...???...";
        bottomPtr = apBuffer;
    }
    else
    {
        // カテゴリの文字列を抽出
        memmove(stringBuffer, apBuffer, (bottomPtr - apBuffer));
        categoryName = codec.toUnicode(&stringBuffer[0], strlen(stringBuffer));
    }

    // カテゴリ名を設定する
    apItemList->setText(0, categoryName);

    // カテゴリ内のURL切り出しを実施する
    char *ptr = bottomPtr;
    while (ptr < (apBuffer + aSize))
    {
        // 板URLを切り出す
        bottomPtr = strstr(ptr, "HREF=");
        if (bottomPtr == NULL)
	{
            break;
        }
        bottomPtr = bottomPtr + sizeof("HREF=") - 1;
        ptr = bottomPtr;
        while ((*ptr > ' ')&&(*ptr != '>'))
        {
            ptr++;
	}
        memset (stringBuffer, 0x00, sizeof(stringBuffer));
        memmove(stringBuffer, bottomPtr, (ptr - bottomPtr));
        while ((*ptr != '>')&&(*ptr != '0x00'))
        {
	   ptr++;
	}
        ptr++;

        // 板の名称文字列を切り出す
        bottomPtr = strstr(ptr, "</A>");

        // bbsBufferにテキストを入れる
        char titleBuffer[200];
        memset (titleBuffer, 0x00, sizeof(titleBuffer));
        memmove(titleBuffer, ptr, (bottomPtr - ptr));
        bbsBuffer = codec.toUnicode(titleBuffer, strlen(titleBuffer));

        if ((strstr(stringBuffer, "2ch.net") != NULL)||
            (strstr(stringBuffer, "bbspink.com") != NULL))
        {
            // 切り出した情報を追加する
            QListViewItem *itemList = new QListViewItem(apItemList);
            itemList->setText(0, bbsBuffer);
            itemList->setText(1, stringBuffer);
            apItemList->insertItem(itemList);
        }
    }
    return;
}

}  // namespace rootj {
