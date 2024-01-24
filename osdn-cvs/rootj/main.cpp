/****************************************************************************
 ** $Id: main.cpp,v 1.1.1.1 2005/04/17 15:07:38 mrsa Exp $
 **
 **  Qt�A�v���P�[�V�����̋N������ (Qt3�p)
 ****************************************************************************/

/* �C���N���[�h�t�@�C���̐ݒ� */
#include <qapplication.h>
#include "MainApp.h"

/* -----------------------------------------------------------------------**
 *   ...�v���g�^�C�v�錾...
 * -----------------------------------------------------------------------**/

/* �����̉�� */
int  parseArgument(int argc, char **argv, MainApp *aWinP);

/* ���C���E�B���h�E�\���O���� */
void prepareWindow(MainApp *aWinP);

/***************************************************************************/
/**  ���C���̋N������                                                     **/
/**                  �iMainApp�N���X���J���ꍇ�A���̕����̕ύX�͕s�v...�j **/
/***************************************************************************/
int main(int argc, char **argv)
{
    /** �A�v���P�[�V�����̃E�B���h�E�쐬 **/
    QApplication apl(argc, argv);
    MainApp *mainWindow = new MainApp;

    /**  �����̉��߃��W�b�N **/
    int result = parseArgument(argc, argv, mainWindow);
    if (result < 0)
    {
        // �����̉��Ɏ��s�A�A�I������
        return (result);
    }

    // ���C���E�B���h�E��ݒ肷��
    apl.setMainWidget(mainWindow);
    prepareWindow(mainWindow);

    // ���C���E�B���h�E�̏���������
    mainWindow->initializeSelf();
    
    // ���C��Window��\������
    mainWindow->show();
    result = apl.exec();

    // ���C���E�B���h�E��filnailze����
    mainWindow->finalizeSelf();
    
    // ���C��Window���폜����
    delete mainWindow;

    return (result);
}

/**************************************************************************
   �����̉�͂��s�� (TODO : ��͂��s���AMainApp�ɏ���n��)
 **************************************************************************/
int parseArgument(int argc, char **argv, MainApp *aWinP)
{
/*****
    if ( argc == 2 && strcmp( argv[1], "-transparent" ) == 0 )
    {
        aWinP->setAutoMask( TRUE );
    }
    aWinP->resize( 100, 100 );
*****/

    return (0);
}

/**************************************************************************
   �EB���h�E�̕\���O������s�� (TODO : �E�B���h�E�̃^�C�g����ݒ肷��)
 **************************************************************************/
void prepareWindow(MainApp *aWinP)
{
/**
    aWinP->setCaption("the Qt Application");
**/
    return;
}
