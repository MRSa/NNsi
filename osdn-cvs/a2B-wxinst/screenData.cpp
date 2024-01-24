/*!
 *   ����������...
 *
 */
#include "screenData.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

/*!
 *  �R���X�g���N�^
 */
screenData::screenData(QWidget *parent)
  : QWidget(parent), wizardForm(0), currentScene(0)
{
  // do nothing!
}

/*!
 *  ��������
 *
 */
void screenData::prepare(Ui::a2bInstaller *apForm)
{
    wizardForm = apForm;
    return;
}

/*!
 *  �A�{�[�g�{�^���������ꂽ�Ƃ��A�A�A
 *
 */
void screenData::doAbort(void)
{
    if (currentScene < 3)
    {
        // �L�����Z�������s���邩�m�F����...
        if (QMessageBox::question(0, tr("Cancel Confirmation"), tr("End program, are you OK?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
        {
            // �I�����Ȃ��A�A�A���ɂ��ǂ�
            return;
        }
    }
    // �A�v���P�[�V�����̏I��...
    QApplication::exit();
    return;
}

/*!
 *  �u�߂�v�{�^�����������Ƃ�...
 *
 */
void screenData::doBack(void)
{
    if (currentScene != 0)
    {
        currentScene--;
    }
    updateScreen();
    return;
}

/*!
 *  �u���ցv�{�^�����������Ƃ�...
 *
 */
void screenData::doNext(void)
{
    // �V�[�P���X�𑝂₷...
    currentScene++;
    updateScreen();
    return;
}

/*!
 *  ������ʂ��J��...
 *
 */
void screenData::doStart(void)
{
    currentScene = 0;
    updateScreen();
    return;
}

/*!
 *  �u�ڍׁv�{�^���������ꂽ�Ƃ��̏���...
 *
 */
void screenData::setDetail(void)
{
     QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
     if (dir.isEmpty() == false)
     {
        // �f�B���N�g���G���A�ɔ��f������
        (wizardForm->lineEdit)->setText(dir);
        editTextFinished();
     }
     return;
}

/*!
 *  ��ʂ�؂�ւ���...
 *
 */
void screenData::updateScreen(void)
{
    switch (currentScene)
    {
      case 3:
      default:
        // �R�s�[���s�̊m�F
        setEndScreen();
        break;

      case 2:
        // �R�s�[���s�̊m�F
        setConfirmCopy();
        break;

      case 1:
        // ���O�i�[�f�B���N�g���̎w��
        setSpecifyDirectory();
        break;

      case 0:
        // �悤�������...
        setStartScreen();
        break;
    }
    return;
}

/*!
 *  �J�n��ʂ̐ݒ�...
 *
 */
void screenData::setStartScreen(void)
{
    // �u���ցv�Ɓu�L�����Z���v�͗L���A�u�߂�v�͖���
    (wizardForm->btnBack)->setEnabled(false);
    (wizardForm->btnNext)->setEnabled(true);
    (wizardForm->btnAbort)->setEnabled(true);

    // �f�B���N�g���֘A�̐ݒ�͕\�����Ȃ�...
    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>Start</head><body>Welcome!</body></html>"));

    return;
}

/*!
 *  �f�B���N�g�����w�肷��
 *
 */
void screenData::setSpecifyDirectory(void)
{
    // �߂�A�i�ށA�L�����Z���͗L��...
    (wizardForm->btnBack)->setEnabled(true);
    (wizardForm->btnNext)->setEnabled(false);
    (wizardForm->btnAbort)->setEnabled(true);

    // �f�B���N�g���֘A�̐ݒ�͕\������...
    (wizardForm->label)->show();
    (wizardForm->lineEdit)->show();
    (wizardForm->detailButton)->show();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>Specify a directory</head><body>Please Specify directory.</body></html>"));

    return;
}

/*!
 *  �R�s�[��̊m�F���s��...
 *
 */
void screenData::setConfirmCopy(void)
{
    (wizardForm->btnBack)->setEnabled(true);
    (wizardForm->btnNext)->setEnabled(true);
    (wizardForm->btnAbort)->setEnabled(true);

    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);

    logDirectory  = (wizardForm->lineEdit)->text();
    if (logDirectory.endsWith("/") == false)
    {
        logDirectory = logDirectory + "/";
    }
    int pos = logDirectory.indexOf(":");  // only windows...
    copyDirectory = logDirectory.left(pos) + ":/PRIVATE/DATA_FOLDER/PC_INOUT/";

    QString body = tr("<html><head>confirmation</head><body>");
    body = body + tr("Are you ready?");
    body = body + tr("<br><hr><UL><LI>");
    body = body + tr("bbstable.html : <B>");
    body = body + logDirectory;
    body = body + tr("</B></LI><LI>");
    body = body + tr("a2B.jad & a2B.jar : <B>");
    body = body + copyDirectory;
    body = body + tr("</B></LI><hr></UL></body></html>");

    (wizardForm->textField)->setHtml(body);

    return;
}

/*!
 *  �I�����
 *
 */
void screenData::setEndScreen(void)
{
     // �R�s�[�̎��s...
    if (executeCopyFiles() == false)
    {
        currentScene--;
        return;
    }

    (wizardForm->btnBack)->hide();
    (wizardForm->btnNext)->hide();
    (wizardForm->btnAbort)->setEnabled(true);
    (wizardForm->btnAbort)->setText(tr("End Program"));

    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>End copy</head><body>Copy End...</body></html>"));

    return;
}

/*!
 *  �e�L�X�g�ҏW���I�������Ƃ�...
 *
 */
void screenData::editTextFinished(void)
{
    QString dir = (wizardForm->lineEdit)->text();
    if (dir.indexOf(":/PRIVATE/DATA_FOLDER") >= 0)
    {
        // ���ɂ����߂�ꍇ...
        (wizardForm->btnNext)->setEnabled(true);
    }
    else
    {
        // ���ɂ����߂Ȃ��ꍇ...
        (wizardForm->btnNext)->setEnabled(false);
    }
    return;
}

/*!
 *  �t�@�C���R�s�[�����̎��s...
 *
 */
bool screenData::executeCopyFiles(void)
{
    // WX310�p�̃f�B���N�g����F������...
    int intPos = logDirectory.indexOf("DATA_FOLDER/");
    QString wx310Dir = "file:///SD:/" + logDirectory.mid(intPos + 12);

    // bbstable.html �̃R�s�[...
    QString bbsTable = logDirectory + "bbstable.html";
    if (QFile::exists(bbsTable) == true)
    {
        // bbstable.html�����ɂ���ꍇ�B�B�B
        if (QMessageBox::question(0, tr("Exists bbstable.html"), tr("bbstable.html exists, overwrite it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            // �㏑������ꍇ...
            if (QFile::exists(bbsTable + ".keep") == true)
            {
                QFile::remove(bbsTable + ".keep");
            }
            QFile::rename(bbsTable, bbsTable + ".keep");
	    QFile::copy("bbstable.html", bbsTable);
        }
    }
    else
    {
        QFile::copy("bbstable.html", bbsTable);
    }

    // a2B.jar�̃R�s�[...
    QString jarFile = copyDirectory + "a2B.jar";
    if (QFile::exists(jarFile) == true)
    {
        QFile::remove(jarFile);
    }
    QFile::copy("a2B.jar", jarFile);

    // a2B.jad�̃R�s�[...
    QString jadFile = copyDirectory + "a2B.jad";
    if (QFile::exists(jadFile) == true)
    {
        QFile::remove(jadFile);
    }

    QFile inputFile("a2B.jad");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }
    QFile outputFile(jadFile);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return (false);
    }

    QTextStream in (&inputFile);
    QTextStream out(&outputFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        int pos = line.indexOf("a2B-Directory:");
        if (pos < 0)
        {
            out << line << endl;
        }
    }
    inputFile.close();

    out << "a2B-Directory: " << wx310Dir << endl;
    out.flush();

    outputFile.close();
    return (true);
}

