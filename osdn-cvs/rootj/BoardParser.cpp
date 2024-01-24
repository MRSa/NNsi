/*****************************************************************************
  $Id: BoardParser.cpp,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       �ꗗ����͂���N���X
 *****************************************************************************/
#include "qiodevice.h"
#include "qsjiscodec.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "qlistview.h"
#include "BoardParser.h"

namespace rootj {

// �R���X�g���N�^
BoardParser::BoardParser() : mpProgress(0)
{
    // ���̂Ƃ��뉽�����Ȃ�
}

// �f�X�g���N�^
BoardParser::~BoardParser()
{
    // ���̂Ƃ��뉽�����Ȃ�
}

// �N���X�̏�����
void BoardParser::initializeSelf(QProgressBar *apProgress)
{
    mpProgress = apProgress;
}

// �ꗗ�̉�͂����s����
bool BoardParser::parse(QIODevice *apSourceData, QString &apName, QListView *apRoot)
{
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

    // �c���[���쐬����
    QListViewItem *treeNode = new QListViewItem(apRoot);
    treeNode->setText(0, apName);

    // ... �����Ŕꗗ�̉�͎����� ...
    result = parseMain(apSourceData, treeNode);

    // ���[�g�ɂȂ�
    apRoot->insertItem(treeNode);

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

// ��͏����̃��C��
bool BoardParser::parseMain(QIODevice *apSourceData, QListViewItem *apRoot)
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
    int readSize = apSourceData->readBlock(temporaryBuffer, (dataSize + 100));
    if (readSize < 0)
    {
        free(temporaryBuffer);
        return (false);
    }

    // �J�e�S���̐擪��������
    char *currentPointer = strstr(temporaryBuffer, "<BR><BR><B>");
    if (currentPointer == NULL)
    {
        // �J�e�S�����Ȃ�����...
        free(temporaryBuffer);
        return (false);
    }
    currentPointer = currentPointer + sizeof("<BR><BR><B>") - 1;

    while (currentPointer < (temporaryBuffer + dataSize))
    {
        // ���̃J�e�S���̐擪��������
        char *nextPointer = strstr(currentPointer, "<BR><BR><B>");
        if (nextPointer == NULL)
        {
            nextPointer = temporaryBuffer + dataSize;
	}

        // �J�e�S���f�[�^�̉��
        QListViewItem *listItem = new QListViewItem(apRoot);
        parseCategory(currentPointer,(nextPointer - currentPointer),listItem);

        // �\���̈�ɂȂ�
        apRoot->insertItem(listItem);

        // ���̃J�e�S���̐擪�ֈړ�...
        currentPointer = nextPointer + sizeof("<BR><BR><B>") - 1;
        mpProgress->setProgress((currentPointer - temporaryBuffer));
    }

    // �m�ۂ����o�b�t�@���J������
    free(temporaryBuffer);
    return (true);
}

// �P�̃J�e�S���̃f�[�^���擾
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
        // �J�e�S�����Ȃ�...(���̃J�e�S����ݒ肷��)
        categoryName = "...???...";
        bottomPtr = apBuffer;
    }
    else
    {
        // �J�e�S���̕�����𒊏o
        memmove(stringBuffer, apBuffer, (bottomPtr - apBuffer));
        categoryName = codec.toUnicode(&stringBuffer[0], strlen(stringBuffer));
    }

    // �J�e�S������ݒ肷��
    apItemList->setText(0, categoryName);

    // �J�e�S������URL�؂�o�������{����
    char *ptr = bottomPtr;
    while (ptr < (apBuffer + aSize))
    {
        // ��URL��؂�o��
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

        // �̖��̕������؂�o��
        bottomPtr = strstr(ptr, "</A>");

        // bbsBuffer�Ƀe�L�X�g������
        char titleBuffer[200];
        memset (titleBuffer, 0x00, sizeof(titleBuffer));
        memmove(titleBuffer, ptr, (bottomPtr - ptr));
        bbsBuffer = codec.toUnicode(titleBuffer, strlen(titleBuffer));

        if ((strstr(stringBuffer, "2ch.net") != NULL)||
            (strstr(stringBuffer, "bbspink.com") != NULL))
        {
            // �؂�o��������ǉ�����
            QListViewItem *itemList = new QListViewItem(apItemList);
            itemList->setText(0, bbsBuffer);
            itemList->setText(1, stringBuffer);
            apItemList->insertItem(itemList);
        }
    }
    return;
}

}  // namespace rootj {
