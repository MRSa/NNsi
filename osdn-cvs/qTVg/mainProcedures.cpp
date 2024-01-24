/*
 *   ��ʐ��䕔..
 *
 */
#include <QCoreApplication>
#include <QtGlobal>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDate>
#include <QTextStream>
#include "aplVersion.h"
#include "mainProcedures.h"

/*
 *  �R���X�g���N�^ (��ʕ\������)
 *
 */
mainProcedures::mainProcedures(Ui::mainForm *uiForm, QWidget *parent) : QWidget(parent)
{
    // �t�H�[�����L�^����
    ui = uiForm;

    // �������\�b�h
    prepare();
}

/*
 *  �f�X�g���N�^ (��ʕ\���̂��Ƃ��܂�)
 *
 */
mainProcedures::~mainProcedures()
{
    delete mpHttpConn2;
    delete mpHttpConn1;
    delete mpHttpConn0;
}

/*
 *  �N���X�̏���...
 *
 */
void mainProcedures::prepare(void)
{
    mpHttpConn0 = new a2BChttpConnection(&mHttpIndex0);
    mpHttpConn1 = new a2BChttpConnection(&mHttpIndex1);
    mpHttpConn2 = new a2BChttpConnection(&mHttpIndex2);

    connect(mpHttpConn0, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));

    connect(mpHttpConn1, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));

    connect(mpHttpConn2, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));


    return;
}

/*
 *  Qt�̏���\��
 *
 */
void mainProcedures::show_aboutQt(bool checked)
{
    // Qt�̃o�[�W��������\������
    QMessageBox::aboutQt(this);
    return;
}

/*
 *  �o�[�W�������̕\��
 *
 */
void mainProcedures::show_version(bool checked)
{
    QString caption = "about " + APL_NAME;
    QString message = APL_CREDIT + "\n" + APL_VERSION + " (" + APL_DATE + ")\n\n" + APL_COMMENT;

    QMessageBox::information(this, caption, message, QMessageBox::Ok);
    return;
}

/*
 *  �A�v���P�[�V�����̃X�^�[�g�I
 *
 */
void mainProcedures::startup(void)
{
    return;
}

/*
 *  �A�v���P�[�V�����̊J�n�I
 *
 */
void mainProcedures::startApp(void)
{
    // �f�[�^�t�@�C���̓ǂݏo��...
    QString dataFileName = QCoreApplication::applicationDirPath() + "/qTVg.dat";
    if (QFile::exists(dataFileName) == false)
    {
        // �f�[�^�t�@�C�����Ȃ�����...�I������
        return;
    }
    QFile dataFile(dataFileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // �f�[�^�t�@�C���I�[�v���Ɏ��s�A�A�A�I������
        return;
    }

    // �f�[�^�t�@�C�����P�s�Âǂݏo��
    int count = 0;
    QTextStream in(&dataFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        extractData(count, line);
        count++;
    }
    dataFile.close();

    // �f�[�^����ʂɔ��f�I
    updateLogInformation();
    return;
}

/*
 *  �f�[�^��ǂݏo���ēW�J����
 */
void mainProcedures::extractData(int lineData, QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        // �f�[�^�Ȃ�...���������I������
    }

    int     index = 0;
    QString dataString = arLineData.section(";", 0, 0);
    switch (lineData)
    {
      case 0:
        // �f�[�^�L�^�f�B���N�g����
        (ui->fldDataDirectory)->setText(dataString);
        break;

      case 1:
        // �n����
        index = (ui->cmbAreaId)->findData(QVariant(dataString.toInt()));
        if (index >= 0)
        {
            (ui->cmbAreaId)->setCurrentIndex(index);
        }
        break;

      default:
        // �������Ȃ�
        break;
    }
    //    QMessageBox::information(0, "", dataFile);
    return;
}

/*
 *  �A�v���P�[�V�����̏I���I
 *
 */
void mainProcedures::finishApp(void)
{
    // �f�[�^�t�@�C���̏����o��...
    QString dataFileName = QCoreApplication::applicationDirPath() + "/qTVg.dat";

    // �Â��t�@�C�����������ꍇ�A�폜����
    if (QFile::exists(dataFileName) == true)
    {
        QFile::remove(dataFileName);
    }

    // �����o���p�ɃI�[�v��
    QFile dataFile(dataFileName);
    if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&dataFile);
    out << (ui->fldDataDirectory)->text() << ";" << endl;

    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();
    out << areaId << ";" << endl;

    out.flush();
    dataFile.close();

    return;
}

/*
 *  �u�f�B���N�g���ݒ�v�{�^�����������Ƃ�...
 *
 */
void mainProcedures::selectDirectory()
{
    QString fileName = (ui->fldDataDirectory)->text();
    fileName = QFileDialog::getExistingDirectory(0, tr("Directory Selection"), fileName);
    if (fileName.isEmpty() != true)
    {
        (ui->fldDataDirectory)->setText(fileName);
    }
    return;
}

/*
 *  �u�f�[�^�擾�v�{�^�����������Ƃ�...
 *
 */
void mainProcedures::getDataFile()
{
    if (QMessageBox::question(0, tr("HTTP COMM."), tr("Data Get, Ready?"), 
        (QMessageBox::Ok | QMessageBox::Cancel)) != QMessageBox::Ok)
    {
        // �L�����Z�����ꂽ...�擾���Ȃ��B�i�I������j
        return;
    }

    // �n��ID���擾����
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // �t�@�C�����w�b�_���쐬����
    QString cookie;
    cookie.setNum(areaId);
    cookie = "areaId=" + cookie + "; span=24";
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString;
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-";
    dateString = (ui->fldDataDirectory)->text() + "/" + dateString;
    QString urlHeader = "http://tv.so-net.ne.jp/guest/pc/chart.action?&span=24&head=" + currentDate.toString("yyyyMMdd") + "0500" + "&index=";

    if ((ui->checkBox_0)->isChecked() == true)
    {
        // �n��g�̃f�[�^�t�@�C�����擾����
        QString dataFileName = dateString + "0.html";
        QString urlToGet = urlHeader + "0";
        mpHttpConn0->setCookie(cookie);
        mpHttpConn0->getFileFromUrl(dataFileName, urlToGet, false);
    }

    if ((ui->checkBox_1)->isChecked() == true)
    {
        // UHF/BS�̃f�[�^�t�@�C�����擾����
        QString dataFileName = dateString + "1.html";
        QString urlToGet = urlHeader + "1";
        mpHttpConn1->setCookie(cookie);
        mpHttpConn1->getFileFromUrl(dataFileName, urlToGet, false);
    }

    if ((ui->checkBox_2)->isChecked() == true)
    {
        // BS�f�W�^���̃f�[�^�t�@�C�����폜����
        QString dataFileName = dateString + "2.html";
        QString urlToGet = urlHeader + "2";
        mpHttpConn2->setCookie(cookie);
        mpHttpConn2->getFileFromUrl(dataFileName, urlToGet, false);
    }

    // �����X�V����
    updateLogInformation();
}

/*
 *  �u�f�[�^�폜�v�{�^�����������Ƃ�...
 *
 */
void mainProcedures::deleteDataFile()
{
    if (QMessageBox::question(0, tr("delete File"), tr("File Delete, OK?"), 
        (QMessageBox::Ok | QMessageBox::Cancel)) != QMessageBox::Ok)
    {
        // �L�����Z�����ꂽ...�폜���Ȃ��B�i�I������j
        return;
    }

    // �n��ID���擾����
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // �t�@�C�����w�b�_���쐬����
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString;
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-";
    dateString = (ui->fldDataDirectory)->text() + "/" + dateString;

    if ((ui->checkBox_0)->isChecked() == true)
    {
        // �n��g�̃f�[�^�t�@�C�����폜����
        QString dataFileName = dateString + "0.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    if ((ui->checkBox_1)->isChecked() == true)
    {
        // UHF/BS�̃f�[�^�t�@�C�����폜����
        QString dataFileName = dateString + "1.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    if ((ui->checkBox_2)->isChecked() == true)
    {
        // BS�f�W�^���̃f�[�^�t�@�C�����폜����
        QString dataFileName = dateString + "2.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    // �����X�V����
    updateLogInformation();
}

/*
 *  �u�����v�{�^�����������Ƃ��̏���...
 *
 */
void mainProcedures::moveToToday()
{
    (ui->calendarWidget)->setSelectedDate(QDate::currentDate());
    updateLogInformation();
}

/*
 *  ��ʏ����X�V����
 *
 */
void mainProcedures::updateLogInformation()
{
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString = currentDate.toString("yyyy/MM/dd");
    (ui->lblDate)->setText(dateString);

    // �n��ID���擾����
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // �n��g�̃t�@�C�������邩�ǂ����H
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "0.html";
    bool isIndex0 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex0 == true)
    {
        (ui->dataIndex_0)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_0)->setText(tr("None"));
    }

    // UHF/BS�̃t�@�C�������邩�H
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "1.html";
    bool isIndex1 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex1 == true)
    {
        (ui->dataIndex_1)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_1)->setText(tr("None"));
    }

    // BS�f�W�^���̃t�@�C�������邩�H
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "2.html";
    bool isIndex2 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex2 == true)
    {
        (ui->dataIndex_2)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_2)->setText(tr("None"));
    }

    // 
    bool enableGetButton = true;
    if (mpHttpConn0 != 0)
    {
       if (mpHttpConn0->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }
    if (mpHttpConn1 != 0)
    {
       if (mpHttpConn1->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }
    if (mpHttpConn2 != 0)
    {
       if (mpHttpConn2->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }

    // ���O�擾�{�^���𐧌䂷��
    (ui->btnGet)->setEnabled(enableGetButton);
    (ui->btnClear)->setEnabled(enableGetButton);
}

/**
 *  �ʐM�I������M�B�B�B
 *
 */
void mainProcedures::completedCommunication(int id, QString &arMsg)
{
    updateLogInformation();
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
