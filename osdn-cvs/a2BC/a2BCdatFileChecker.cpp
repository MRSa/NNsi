/*!
 *   a2BCdatFileChecker
 * 
 *    [����]
 * 
 * 
 */
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include "a2BCdatFileChecker.h"
#include "a2BCsubjectParser.h"
#include "datFileUtil.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCdatFileChecker::a2BCdatFileChecker(QWidget *parent)
 : QWidget(parent)
{
    resetData();
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BCdatFileChecker::~a2BCdatFileChecker()
{
    resetData();
}


/*!
 *  �����i�[�f�[�^�̃��Z�b�g
 * 
 * 
 */
void a2BCdatFileChecker::resetData(void)
{
    mDatFileTitle.clear();
    mDatFileName.clear();
    mPath.clear();    
    return;
}

/*!
 *  �f�[�^�t�@�C�������L������
 * 
 */
bool a2BCdatFileChecker::appendDataFile(const QString &arPath, const QString &arDatFileName)
{
    datFileUtil checker;
    if (arDatFileName.contains(".bak") == true)
    {
        // �o�b�N�A�b�v�t�@�C���������A�A�A�o�^���Ȃ��B
        return (false);
    }

    // �t�@�C���Ɋi�[����Ă���X���^�C�g���𒊏o����
    QString title = checker.pickupDatFileTitle(arPath, arDatFileName);

    mDatFileTitle.append(title);
    mDatFileName.append(arDatFileName);
    mPath.append(arPath);

    return (true);
}

/*!
 *   a2b.idx�փf�[�^���o�͂���
 * 
 *   �� ���܂�ɂ�����...�v�ύX
 * 
 */
void a2BCdatFileChecker::outputDataFile(QString &arPath, QTextStream *apStream)
{
    int limit = mDatFileName.size();
    for (int index = 0; index < limit; index++)
    {
        if (mPath[index] == arPath)
        {
            (*apStream) << mDatFileName[index] << ",0,1,6,0,0," << mDatFileTitle[index] << endl;
        }
    }
    return;
}

/*!
 *   subject.txt�Ǘ��t�@�C���ɒǉ�����...
 * 
 *   �� ���܂�ɂ�����...�v�ύX
 * 
 */
void a2BCdatFileChecker::appendSubjectTxtFile(QString &arPath, a2BCsubjectParser *apParser)
{
    if (apParser->isPrepare() != true)
    {
        return;
    }

    int limit = mDatFileName.size();
    for (int index = 0; index < limit; index++)
    {
        if (mPath[index] == arPath)
        {
            if (apParser->exist(mDatFileName[index]) == false)
            {
                //
                apParser->appendData(mDatFileName[index], mDatFileTitle[index]);
            }
        }
    }
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
