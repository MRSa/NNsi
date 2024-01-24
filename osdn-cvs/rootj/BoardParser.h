/*****************************************************************************
  $Id: BoardParser.h,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       板一覧を解析するクラス
 *****************************************************************************/
#ifndef ROOTJ_BOARD_PARSER_H
#define ROOTJ_BOARD_PARSER_H

class QIODevice;
class QProgressBar;
class QListView;
class QListViewItem;
namespace rootj {

class BoardParser
{
public:
    // コンストラクタ
    BoardParser();

    // デストラクタ
    ~BoardParser();

public:
    // クラスの初期化
    void initializeSelf(QProgressBar *apProgress);

    // 板一覧解析の実行
    bool parse(QIODevice *apSourceData, QString &apName, QListView *apRoot);

private:
    // 板一覧を解析する
    bool parseMain(QIODevice *apSourceData, QListViewItem *apRoot);

    // 板の1カテゴリを解析する
    void parseCategory(char *apBuffer, int aSize, QListViewItem *apItemList);
                                   

private:
    QProgressBar  *mpProgress;
};

}  // namespace rootj {
#endif  // #define ROOTJ_BOARD_PARSER_H
