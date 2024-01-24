/*****************************************************************************
  $Id: ThreadParser.h,v 1.1 2005/04/24 13:20:16 mrsa Exp $
       �X���ꗗ����͂���N���X
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
    // �R���X�g���N�^
    ThreadParser();

    // �f�X�g���N�^
    ~ThreadParser();

public:
    // �N���X�̏�����
    void initializeSelf(QProgressBar *apProgress);

    // �X���ꗗ��͂̎��s
    bool parse(QIODevice *apSourceData, QTable *apRoot);

private:
    // �X���ꗗ����͂���
    bool parseMain(QIODevice *apSourceData, QTable *apRoot);

    // �X���̏�����͂���
    void parseThreadInfo(char *apBuffer, int aSize, QTable *apTable, int aRow);

private:
    QProgressBar  *mpProgress;
};

}  // namespace rootj {
#endif  // #define ROOTJ_THREAD_PARSER_H


