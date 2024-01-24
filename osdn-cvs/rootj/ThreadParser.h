/*****************************************************************************
  $Id: ThreadParser.h,v 1.1 2005/04/24 13:20:16 mrsa Exp $
       スレ一覧を解析するクラス
 *****************************************************************************/
#ifndef ROOTJ_THREAD_PARSER_H
#define ROOTJ_THREAD_PARSER_H

class QIODevice;
class QProgressBar;
class QTable;
class QTableItem;
namespace rootj {

class ThreadParser
{

public:
    // コンストラクタ
    ThreadParser();

    // デストラクタ
    ~ThreadParser();

public:
    // クラスの初期化
    void initializeSelf(QProgressBar *apProgress);

    // スレ一覧解析の実行
    bool parse(QIODevice *apSourceData, QTable *apRoot);

private:
    // スレ一覧を解析する
    bool parseMain(QIODevice *apSourceData, QTable *apRoot);

    // スレの情報を解析する
    void parseThreadInfo(char *apBuffer, int aSize, QTable *apTable, int aRow);

private:
    QProgressBar  *mpProgress;
};

}  // namespace rootj {
#endif  // #define ROOTJ_THREAD_PARSER_H


