/*****************************************************************************
  $Id: BoardParser.h,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       �ꗗ����͂���N���X
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
    // �R���X�g���N�^
    BoardParser();

    // �f�X�g���N�^
    ~BoardParser();

public:
    // �N���X�̏�����
    void initializeSelf(QProgressBar *apProgress);

    // �ꗗ��͂̎��s
    bool parse(QIODevice *apSourceData, QString &apName, QListView *apRoot);

private:
    // �ꗗ����͂���
    bool parseMain(QIODevice *apSourceData, QListViewItem *apRoot);

    // ��1�J�e�S������͂���
    void parseCategory(char *apBuffer, int aSize, QListViewItem *apItemList);
                                   

private:
    QProgressBar  *mpProgress;
};

}  // namespace rootj {
#endif  // #define ROOTJ_BOARD_PARSER_H
