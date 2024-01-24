/*****************************************************************************
  $Id: ThreadParser.cpp,v 1.1 2005/04/24 13:20:16 mrsa Exp $
       �X���ꗗ����͂���N���X
 *****************************************************************************/
#include "qiodevice.h"
#include "qsjiscodec.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "qtable.h"
#include "ThreadParser.h"

namespace rootj {

// �R���X�g���N�^
ThreadParser::ThreadParser() : mpProgress(0)
{
    // ���̂Ƃ��뉽�����Ȃ�
}

// �f�X�g���N�^
ThreadParser::~ThreadParser()
{
    // ���̂Ƃ��뉽�����Ȃ�
}

// �N���X�̏�����
void ThreadParser::initializeSelf(QProgressBar *apProgress)
{
    mpProgress = apProgress;
}

// �ꗗ�̉�͂����s����
bool ThreadParser::parse(QIODevice *apSourceData, QTable *apRoot)
{
    //QMessageBox::information(NULL,QObject::tr("information"),QObject::tr("ThreadParser::parse()"),QMessageBox::Ok);

    bool result = false;
    if ((apSourceData == 0)||(apRoot == 0))
    {
        // �����̎w�肪�������Ȃ��A�I������
        return (result);
    }

    // (�{���Ȃ��͂�����)�f�[�^�\�[�X���I�[�v������Ă���΁A��������close
    if (apSourceData->isOpen() == true)
    {
        apSourceData->close();
    }

    // �f�[�^�\�[�X���I�[�v������
    apSourceData->open(IO_ReadOnly);

    // �擪��...
    apSourceData->reset();

    // �v���O���X�o�[��(�g�p�\��)����������
    if (mpProgress != 0)
    {
        mpProgress->setEnabled(true);
        mpProgress->reset();
    }
    int bufSize = apSourceData->size();
    mpProgress->setTotalSteps(bufSize);
    mpProgress->setProgress(0);

    // ... �����ŃX���ꗗ�̉�͎����� ...
    result = parseMain(apSourceData, apRoot);

    // �f�[�^�\�[�X���N���[�Y����
    apSourceData->close();

    // �v���O���X�o�[��(�g���I������̂�)�g�p�s�ɂ���
    if (mpProgress != 0)
    {
        mpProgress->reset();
        mpProgress->setEnabled(false);
    }
    return (result);
}

// ��͏����̃��C������
bool ThreadParser::parseMain(QIODevice *apSourceData, QTable *apRoot)
{
    // �f�[�^�T�C�Y���擾����
    int dataSize = apSourceData->size();

    // �o�b�t�@���m�ۂ���B�B�B
    char *temporaryBuffer = static_cast<char *> (malloc(dataSize + 100));
    if (temporaryBuffer == NULL)
    {
        return (false);
    }
    memset(temporaryBuffer, 0x00, (dataSize + 100));

    // �f�[�^����C�ɓǂݍ���
    int readSize = apSourceData->readBlock(temporaryBuffer, (dataSize + 100));
    if (readSize < 0)
    {
        free(temporaryBuffer);
        return (false);
    }
    // �f�[�^�̖�����������
    char *topPointer    = temporaryBuffer;
    char *bottomPointer = topPointer;
    int   row           = 0;

    while (bottomPointer < (temporaryBuffer + dataSize))
    {
        bottomPointer = strstr(topPointer, "\x0a");
        if (bottomPointer == NULL)
        {
            // �f�[�^���Ȃ��Ȃ����A������
            break;
	}
        // ��s�ǉ�����...
        apRoot->insertRows(row);

        // �X��������͂���B
        parseThreadInfo(topPointer, (bottomPointer - topPointer), apRoot, row);

        // ���̍��ڂւ����߂�B
        topPointer = bottomPointer + 1;
        row++;

        // �v���O���X�o�[���X�V����
        mpProgress->setProgress(topPointer - temporaryBuffer);
    }

    // �m�ۂ����o�b�t�@���J�����ďI������
    free(temporaryBuffer);

    return (true);
}

// �P�̃J�e�S���̃f�[�^���擾
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


