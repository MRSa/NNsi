/*!
 *   ��ʐ��䕔..
 *
 */
#include <QtGlobal>
#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include "screenControl.h"

/*!
 *   �R���X�g���N�^
 *
 */
screenControl::screenControl(Ui::a2bInstaller *apForm, QWidget *parent)
  : QWidget(parent), mpForm(apForm)
{
    mDB.prepare(apForm);
    connectSignals();
}

/*!
 *  connectSignals()  : �V�O�i���ƃX���b�g���Ȃ�����...
 *
 */
void screenControl::connectSignals(void)
{
    qDebug("screenControl::connectSignals()");

    // Abort�{�^���������ꂽ�Ƃ�...
    connect(mpForm->btnAbort, SIGNAL(clicked()),
            this,             SLOT(clicked_ButtonAbort()));

    // �߂�{�^���������ꂽ�Ƃ�...
    connect(mpForm->btnBack, SIGNAL(clicked()),
            this,            SLOT(clicked_ButtonBack()));

    // ���փ{�^���������ꂽ�Ƃ�...
    connect(mpForm->btnNext, SIGNAL(clicked()),
            this,            SLOT(clicked_ButtonNext()));

    // �Q�ƃ{�^���������ꂽ�Ƃ�...
    connect(mpForm->detailButton, SIGNAL(clicked()),
            this,                 SLOT(clicked_detailButton()));

    // �e�L�X�g���X�V���ꂽ�Ƃ�...
    connect(mpForm->lineEdit, SIGNAL(returnPressed()),
            this,             SLOT(clicked_EditText()));
    connect(mpForm->lineEdit, SIGNAL(editingFinished()),
            this,             SLOT(clicked_EditText()));

    return;
}

/*!
 *    �{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::clicked_ButtonAbort()
{
    qDebug("screenControl::clicked_ButtonAbort()");

    mDB.doAbort();
    return;
}

/*!
 *    �߂�{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::clicked_ButtonBack()
{
    qDebug("screenControl::clicked_ButtonBack()");

    mDB.doBack();
    return;
}

/*!
 *    ���փ{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::clicked_ButtonNext()
{
    qDebug("screenControl::clicked_ButtonNext()");

    mDB.doNext();
    return;
}

/*!
 *    �Q�ƃ{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::clicked_detailButton()
{
    qDebug("screenControl::clicked_detailButton()");

    mDB.setDetail();
    return;
}

/*!
 *   Qt �̃N���W�b�g��\������
 *
 */
void screenControl::aboutQt()
{
    qDebug("screenControl::aboutQt()");

    QApplication::aboutQt();
    return;
}

/*!
 *  �K�v�ȃt�@�C�������݂��邩�m�F����
 *
 */
bool  screenControl::checkFilesExist(void)
{
    // a2B.jad�̑��݃`�F�b�N
    if (QFile::exists("a2B.jad") == false)
    {
        QMessageBox::critical(0, tr("ERROR(jad)"), tr("Cannot find a2B.jad."));
        return (false);
    }

    // a2B.jar�̑��݃`�F�b�N
    if (QFile::exists("a2B.jar") == false)
    {
        QMessageBox::critical(0, tr("ERROR(jar)"), tr("Cannot find a2B.jar."));
        return (false);
    }

    // bbstable.html�̑��݃`�F�b�N
    if (QFile::exists("bbstable.html") == false)
    {
        QMessageBox::critical(0, tr("ERROR(bbstable)"), tr("Cannot find bbstable.html."));
        return (false);
    }
    return (true);
}

/*!
 *    �e�L�X�g�ҏW���s��ꂽ�Ƃ��̏���
 *
 */
void screenControl::clicked_EditText()
{
    qDebug("screenControl::clicked_EditText()");

    mDB.editTextFinished();
    return;
}

/*!
 *  ��ʂ̏�����
 *
 */
void screenControl::prepare(void)
{
    mDB.doStart();
    return;
}
