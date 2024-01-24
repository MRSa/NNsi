/*!
 *   �f�[�^�ێ���..
 *
 */
#include "screenData.h"

/*!
 *  �R���X�g���N�^
 *
 */
screenData::screenData()
{
    bbsTableUrl              = "http://menu.2ch.net/bbstable.html";
    bbsTableFileName         = "";
    otherBrowserDirectory    = "";
    updateSubjectTxt         = Qt::Checked;
    enableDownThread         = Qt::Unchecked;
    addThreadToIndex         = Qt::Unchecked;
    addToBbsTable            = Qt::Unchecked;
    disableGetPart           = Qt::Unchecked;
    removeErrorThreadMessage = Qt::Unchecked;
    removeOverThreadMessage  = Qt::Unchecked;
    backupErrorLog           = Qt::Unchecked;
    copySubjectTxt           = Qt::Unchecked;
    getInterval              = 500;
    otherBrowserType         = 0;
    synchronizeType          = 0;

}

/*!
 *   bbstable.html�̎擾��URL����������
 *
 */
QString screenData::getBbsTableUrl(void)
{
    return (bbsTableUrl);
}

/*!
 *   bbstable.html�̎擾��URL���L������
 *
 */
void screenData::setBbsTableUrl(const QString &arUrl)
{
    bbsTableUrl = arUrl;
    return;
}

/*!
 *   bbstable.html�̃t�@�C��������������
 *
 */
QString screenData::getBbsTableFileName(void)
{
    return (bbsTableFileName);
}

/*!
 *  ��2�����u���E�U�̊Ǘ��f�B���N�g������������
 * 
 */
QString screenData::getOtherBrowserDirectory(void)
{
    return (otherBrowserDirectory);
}

/*!
 *  ��2�����u���E�U�̊Ǘ��f�B���N�g�����L������
 * 
 */
void screenData::setOtherBrowserDirectory(const QString &arDirectory)
{
    otherBrowserDirectory = arDirectory;
    return;
}


/*!
 *   bbstable.html�̃t�@�C�������L������
 *
 */
void screenData::setBbsTableFileName(const QString &arFileName)
{
    bbsTableFileName = arFileName;
    return;
}

/*!
 *   subject.txt���ŐV�ɍX�V���邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getUpdateSubjectTxt(void)
{
    return (updateSubjectTxt);
}

/*!
 *   subject.txt���ŐV�ɍX�V���邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setUpdateSubjectTxt(Qt::CheckState yesNo)
{
    updateSubjectTxt = yesNo;
}

/*!
 *   �V���m�F�����C�ɓ���ꗗ�����ɂ��邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getEnableDownThread(void)
{
    return (enableDownThread);
}

/*!
 *   �V���m�F�����C�ɓ���ꗗ�����ɂ��邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setEnableDownThread(Qt::CheckState yesNo)
{
    enableDownThread = yesNo;
}

/*!
 *   ���q�X�����Ǘ��ΏۂƂ��邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getAddThreadToIndex(void)
{
    return (addThreadToIndex);
}

/*!
 *   �Ǘ��ΏۃX����subject.txt�ɂȂ������ꍇ�A�ǉ����邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setAddThreadToIndex(Qt::CheckState yesNo)
{
    addThreadToIndex = yesNo;
}

/*!
 *   �ǎ擾�X����������u���C�ɓ���v�Ƃ���bbstable.html�ɒǉ����邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getAddToBbsTable(void)
{
    return (addToBbsTable);
}

/*!
 *  �擾�X����������u���C�ɓ���v�Ƃ���bbstable.html�ɒǉ����邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setAddToBbsTable(Qt::CheckState yesNo)
{
    addToBbsTable = yesNo;
}

/*!
 *   �X�������ׂčĎ擾���邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getDisableGetPart(void)
{
    return (disableGetPart);
}

/*!
 *   �X�������ׂčĎ擾���邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setDisableGetPart(Qt::CheckState yesNo)
{
    disableGetPart = yesNo;
}

/*!
 *   �G���[�X�����Ǘ��Ώۂ���͂������H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getRemoveErrorMessage(void)
{
    return (removeErrorThreadMessage);
}

/*!
 *   �G���[�X�����Ǘ��Ώۂ���͂������H�̏�Ԃ��X�V����
 *
 */
void screenData::setRemoveErrorMessage(Qt::CheckState yesNo)
{
    removeErrorThreadMessage = yesNo;
}

/*!
 *   1000�����X�����Ǘ��Ώۂ���͂������H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getRemoveOverMessage(void)
{
    return (removeOverThreadMessage);
}

/*!
 *   1000�����X�����Ǘ��Ώۂ���͂������H�̏�Ԃ��X�V����
 *
 */
void screenData::setRemoveOverMessage(Qt::CheckState yesNo)
{
    removeOverThreadMessage = yesNo;
}


/*!
 *   �V���m�F�̎擾�Ԋu����������i�P�ʁFms�j
 *
 */
int screenData::getReceiveInterval(void)
{
    return (getInterval);
}


/*!
 *   �V���m�F�̎擾�Ԋu���X�V����i�P�ʁFms�j
 *
 */
void screenData::setReceiveInterval(int interval)
{
    getInterval = (interval < 100) ? 100: interval;
    return;
}

/*!
 *  a2B�ŃG���[���o�����O���ړ�������ݒ�
 * 
 */
Qt::CheckState screenData::getBackupErrorLog(void)
{
    return (backupErrorLog);
}

/*!
 *   a2B�ŃG���[���o�����O���ړ�������ݒ�
 * 
 */
void screenData::setBackupErrorLog(Qt::CheckState yesNo)
{
    backupErrorLog = yesNo;
}

/*!
 *  a2B��1000�𒴂����X�����ړ�������ݒ�
 * 
 */
Qt::CheckState screenData::getBackupOverLog(void)
{
    return (backupOverLog);
}

/*!
 *   a2B��1000�𒴂����X�����ړ�������ݒ�
 * 
 */
void screenData::setBackupOverLog(Qt::CheckState yesNo)
{
    backupOverLog = yesNo;
}

/*!
 *   subject.txt�������ɃR�s�[���邩�H�̏�Ԃ���������
 *
 */
Qt::CheckState screenData::getCopySubjectTxt(void)
{
    return (copySubjectTxt);
}

/*!
 *   subject.txt�������ɃR�s�[���邩�H�̏�Ԃ��X�V����
 *
 */
void screenData::setCopySubjectTxt(Qt::CheckState yesNo)
{
    copySubjectTxt = yesNo;
}

/*!
 *  ���Q�����˂�u���E�U�̃^�C�v���擾����
 * 
 */
int screenData::getOtherBrowserType(void)
{
    return (otherBrowserType);
}

/*!
 *  ���Q�����˂�u���E�U�̃^�C�v��ݒ肷��
 * 
 */
void screenData::setOtherBrowserType(int browserType)
{
    otherBrowserType = browserType;
    return;  
}

/*!
 *  ���Q�����˂�u���E�U�̃^�C�v���擾����
 * 
 */
int screenData::getSynchronizeType(void)
{
    return (synchronizeType);
}

/*!
 *  ���Q�����˂�u���E�U�̃^�C�v��ݒ肷��
 * 
 */
void screenData::setSynchronizeType(int browserType)
{
    synchronizeType = browserType;
    return;  
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
