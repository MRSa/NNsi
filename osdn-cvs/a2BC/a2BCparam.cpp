/*!
 *   �p�����[�^�ǂݏo���E�L����
 *
 */
#include <Qt>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QXmlSimpleReader>
#include "a2BCparam.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCparam::a2BCparam(QWidget *parent)
  : QWidget(parent), mLoaded(false), mItemParam(false), mPath(0), mpDB(0)
{

}

/*!
 *   �f�X�g���N�^
 * 
 */
a2BCparam::~a2BCparam()
{

}

/*!
 *  �t�@�C������ǂݏo��
 * 
 */
bool a2BCparam::loadFromFile(QString &execPath, screenData *arDB)
{
    bool ret = false;
    mpDB  = arDB;
    mPath = execPath;

    QFile file(mPath + "/a2BC-conf.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        // �t�@�C���I�[�v�����s...
        return (false);
    }
    QString     itemName  = "";
    QString     itemType  = "";
    QString     itemValue = "";
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<NNsi:item", Qt::CaseInsensitive) == true)
        {
            mItemParam = true;
            itemName = line.section("\"", 1, 1);
            itemType = line.section("\"", 3, 3);
        }
        if ((mItemParam == true)&&(line.contains("<NNsi:val>", Qt::CaseInsensitive) == true))
        {
            itemValue  = line.section("\"", 1, 1);
            bool result = setParameter(itemName, itemType, itemValue);
            if (result == true)
            {
                ret = true;
            }
            itemName   = "";
            itemType   = "";
            itemValue  = "";
            mItemParam = false;
        }
    }
    file.close();
    return (ret);
}

/*!
 *   �t�@�C���֋L�^����
 * 
 */
bool a2BCparam::saveToFile(void)
{
    if (mpDB == 0)
    {
        return (false);
    }

    QFile file(mPath + "/a2BC-conf.xml");
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        // �t�@�C���I�[�v�����s
        QMessageBox::warning(this, tr("a2BC preference"),
                             tr("Cannot write file %1:\n%2.")
                             .arg("a2BC-conf.xml")
                             .arg(file.errorString()));
        return (false);
    }
    QTextStream out;
    out.setDevice(&file);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE NNsi:NNsiXML SYSTEM \"http://nnsi.sourceforge.jp/NNsiXML/NNsiXML100.dtd\">\n"
        << "<NNsi:NNsiXML xmlns:NNsi=\"http:nnsi.sourceforge.jp/NNsiXML\">\n"
        << "  <NNsi:NNsiSet ver=\"100\">\n";
 
    //////////  bbstable.html �擾��URL  //////////
    out << "    <NNsi:item name=\"bbsTableUrl\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getBbsTableUrl() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  bbstable.html ���[�J���i�[�t�@�C��  //////////
    out << "    <NNsi:item name=\"bbsTableFileName\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getBbsTableFileName() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  �擾�Ԋu (�P�ʁFms)  //////////
    out << "    <NNsi:item name=\"getReceiveInterval\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getReceiveInterval() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  �X���ꗗ�𓯎��ɍX�V���� (true: �X�V����, false: �X�V���Ȃ�)  //////////
    out << "    <NNsi:item name=\"updateSubjectTxt\" type=\"bool\">\n";
    if (mpDB->getUpdateSubjectTxt() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  dat���������X�����ǂ߂�悤�ɂ��� (true:�ǂ߂�悤�ɂ���, false:���Ȃ�) //////////
    out << "    <NNsi:item name=\"enableDownThread\" type=\"bool\">\n";
    if (mpDB->getEnableDownThread() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  (���g�p�A���q�X�����Ǘ��Ώۂɂ���)  //////////
    out << "    <NNsi:item name=\"addThreadToIndex\" type=\"bool\">\n";
    if (mpDB->getAddThreadToIndex() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  �G���[�����������X���𑼃u���E�U�Ɉړ�������  //////////
    out << "    <NNsi:item name=\"backupErrorLog\" type=\"bool\">\n";
    if (mpDB->getBackupErrorLog() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  1000���������������X���𑼃u���E�U�Ɉړ�������  //////////
    out << "    <NNsi:item name=\"backupOverLog\" type=\"bool\">\n";
    if (mpDB->getBackupOverLog() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  bbstable.html��a2B�J�e�S����ǉ����� (true:�ǉ�����, false:�ǉ����Ȃ�)  //////////
    out << "    <NNsi:item name=\"addToBbsTable\" type=\"bool\">\n";
    if (mpDB->getAddToBbsTable() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  �X����S�čĎ擾���� (true:�Ď擾����, false:�Ď擾���Ȃ�)  //////////
    out << "    <NNsi:item name=\"disableGetPart\" type=\"bool\">\n";
    if (mpDB->getDisableGetPart() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  �G���[�X�����Ǘ��Ώۂ���͂��� (true:�O��, false:�O���Ȃ�)  //////////
    out << "    <NNsi:item name=\"removeErrorThreadMessage\" type=\"bool\">\n";
    if (mpDB->getRemoveErrorMessage() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  1000�����X�����Ǘ��Ώۂ���͂��� (true:�O��, false:�O���Ȃ�)  //////////
    out << "    <NNsi:item name=\"removeOverThreadMessage\" type=\"bool\">\n";
    if (mpDB->getRemoveOverMessage() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  �X���ꗗ�𓯎��ɃR�s�[���� (true: �R�s�[����, false: �R�s�[���Ȃ�)  //////////
    out << "    <NNsi:item name=\"copySubjectTxt\" type=\"bool\">\n";
    if (mpDB->getCopySubjectTxt() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  ���Q�����u���E�U�̊Ǘ��f�B���N�g��  //////////
    out << "    <NNsi:item name=\"otherBrowserDirectory\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getOtherBrowserDirectory() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  ���Q�����u���E�U�̃^�C�v  //////////
    out << "    <NNsi:item name=\"otherBrowserType\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getOtherBrowserType() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  ���Q�����u���E�U�Ƃ̓�������  //////////
    out << "    <NNsi:item name=\"synchronizeType\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getSynchronizeType() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    out << "  </NNsi:NNsiSet>\n"
        << "</NNsi:NNsiXML>\n";
    file.flush();
    file.close();
    return (true);
}

/*!
 *   �p�����[�^��ݒ肷��
 * 
 */
bool a2BCparam::setParameter(QString name, QString type, QString value)
{
    bool ret = false;
    
    //////////  bbstable.html �擾��URL  //////////
    if (name.contains("bbsTableUrl", Qt::CaseInsensitive) == true)
    {
        mpDB->setBbsTableUrl(value);
    }

    //////////  bbstable.html ���[�J���i�[�t�@�C��  //////////
    if (name.contains("bbsTableFileName", Qt::CaseInsensitive) == true)
    {
        mpDB->setBbsTableFileName(value);
        if (value.contains("bbstable.html") == true)
        {
            ret = true;
        }
    }

    //////////  �擾�Ԋu (�P�ʁFms)  //////////
    if (name.contains("getReceiveInterval", Qt::CaseInsensitive) == true)
    {
        mpDB->setReceiveInterval(value.toInt());
    }

    //////////  �X���ꗗ�𓯎��ɍX�V���� (true: �X�V����, false: �X�V���Ȃ�)  //////////
    if (name.contains("updateSubjectTxt", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setUpdateSubjectTxt(yesNo);
    }

    //////////  dat���������X�����ǂ߂�悤�ɂ��� (true:�ǂ߂�悤�ɂ���, false:���Ȃ�) //////////
    if (name.contains("enableDownThread", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setEnableDownThread(yesNo);
    }

    //////////  (���g�p�A���q�X�����Ǘ��Ώۂɂ���)  //////////
    if (name.contains("addThreadToIndex", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setAddThreadToIndex(yesNo);
    }

    //////////  �G���[�����������X���𑼃u���E�U�Ɉړ������� (true:�ړ�����, false:�ړ����Ȃ�)  //////////
    if (name.contains("backupErrorLog", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setBackupErrorLog(yesNo);
    }

    //////////  1000���������������X���𑼃u���E�U�Ɉړ������� (true:�ړ�����, false:�ړ����Ȃ�)  //////////
    if (name.contains("backupOverLog", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setBackupOverLog(yesNo);
    }

    //////////  bbstable.html��a2B�J�e�S����ǉ����� (true:�ǉ�����, false:�ǉ����Ȃ�)  //////////
    if (name.contains("addToBbsTable", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setAddToBbsTable(yesNo);
    }

    //////////  �X����S�čĎ擾���� (true:�Ď擾����, false:�Ď擾���Ȃ�)  //////////
    if (name.contains("disableGetPart", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setDisableGetPart(yesNo);
    }

    //////////  �G���[�X�����Ǘ��Ώۂ���͂��� (true:�O��, false:�O���Ȃ�)  //////////
    if (name.contains("removeErrorThreadMessage", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setRemoveErrorMessage(yesNo);
    }

    //////////  1000�����X�����Ǘ��Ώۂ���͂��� (true:�O��, false:�O���Ȃ�)  //////////
    if (name.contains("removeOverThreadMessage", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setRemoveOverMessage(yesNo);
    }

    //////////  �X���ꗗ�𓯎��ɍX�V���� (true: �X�V����, false: �X�V���Ȃ�)  //////////
    if (name.contains("copySubjectTxt", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setCopySubjectTxt(yesNo);
    }

    //////////  ���Q�����u���E�U�̊Ǘ��f�B���N�g��  //////////
    if (name.contains("otherBrowserDirectory", Qt::CaseInsensitive) == true)
    {
        mpDB->setOtherBrowserDirectory(value);
    }

    //////////  ���Q�����u���E�U�̃^�C�v  //////////
    if (name.contains("otherBrowserType", Qt::CaseInsensitive) == true)
    {
        mpDB->setOtherBrowserType(value.toInt());
    }

    //////////  ���Q�����u���E�U�̃^�C�v  //////////
    if (name.contains("synchronizeType", Qt::CaseInsensitive) == true)
    {
        mpDB->setSynchronizeType(value.toInt());
    }

    return (ret);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
