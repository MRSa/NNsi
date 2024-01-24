/*
 *   ��ʐ��䕔..
 *
 */
#include <QtGlobal>
#include "mainControl.h"

/*
 *  �R���X�g���N�^ (��ʕ\������)
 *
 */
mainControl::mainControl(QWidget *parent) : QWidget(parent)
{
    // �����ł͉������Ȃ�
}

/*
 *  �f�X�g���N�^ (��ʕ\���̂��Ƃ��܂�)
 *
 */
mainControl::~mainControl()
{
    // �����ł͉������Ȃ�
}

/*
 *   ���C����ʂ̕\������
 *
 */
void mainControl::setupMainUi(QMainWindow *apWidget)
{
    // ���C��Widget��ݒ肵�Ă���V�O�i���ƃX���b�g��ڑ�����
    prepareObjects();
    mpMainForm->setupUi(apWidget);
    prepareDatas();
    connectSignals();

    // �f�[�^�̓W�J
    startup();

    return;
}

/*
 *  �|��t�@�C�����̎擾
 *
 */
QString mainControl::getTranslateFileName(void)
{
    /*
     *  �����Ŗ|��t�@�C�������w�肷��
     *
     */
    return ("qTVg_jp");
}

/*
 *  �N������
 *
 */
void mainControl::startApp(int argc, char *argv[])
{
    // �A�v���P�[�V�����̊J�n����...
    mpMainProcedures->startApp();
    return;
}

/*
 *  �I������
 *
 */
void mainControl::finishApp(void)
{
    // �A�v���P�[�V�����̏I������...
    mpMainProcedures->finishApp();
    return;
}

/*
 *  �I�u�W�F�N�g�̐���
 *
 */
void mainControl::prepareObjects(void)
{
    mpMainForm       = new Ui::mainForm();
    mpMainProcedures = new mainProcedures(mpMainForm);
    return;
}

/*
 *  �V�O�i���ƃX���b�g�̐ڑ�
 *
 */
void mainControl::connectSignals(void)
{
    // About Qt�̕\��
    connect(mpMainForm->actionAbout_Qt, SIGNAL(triggered(bool)),
            mpMainProcedures,           SLOT(show_aboutQt(bool)));

    // �o�[�W�������̕\��
    connect(mpMainForm->actionShow_Version, SIGNAL(triggered(bool)),
            mpMainProcedures,               SLOT(show_version(bool)));

    // ���O�̎擾���s
    connect(mpMainForm->btnGet, SIGNAL(clicked()),
            mpMainProcedures,   SLOT(getDataFile()));

    // ���O�t�@�C���̍폜
    connect(mpMainForm->btnClear, SIGNAL(clicked()),
            mpMainProcedures,     SLOT(deleteDataFile()));

    // ���O�L�^�f�B���N�g���̎w��
    connect(mpMainForm->btnSelectDir, SIGNAL(clicked()),
            mpMainProcedures,         SLOT(selectDirectory()));

    // �J�����_�[�E�B�W�F�b�g���X�V���ꂽ�Ƃ�...
    connect(mpMainForm->btnToday,       SIGNAL(clicked()),
            mpMainProcedures,           SLOT(moveToToday()));
    connect(mpMainForm->calendarWidget, SIGNAL(clicked()),
            mpMainProcedures,           SLOT(updateLogInformation()));
    connect(mpMainForm->calendarWidget, SIGNAL(activated()),
            mpMainProcedures,           SLOT(updateLogInformation()));
    connect(mpMainForm->calendarWidget, SIGNAL(selectionChanged()),
            mpMainProcedures,           SLOT(updateLogInformation()));

    return;
}


/*
 *  �f�[�^�̐ݒ�i�n��f�[�^��ݒ肷��j
 *
 */
void mainControl::prepareDatas(void)
{
    // �n��I���̃R���{�{�b�N�X�ɃA�C�e����}������
    (mpMainForm->cmbAreaId)->addItem(tr("Sapporo"),     QVariant(1));
    (mpMainForm->cmbAreaId)->addItem(tr("Hakodate"),    QVariant(52));
    (mpMainForm->cmbAreaId)->addItem(tr("Asahikawa"),   QVariant(48));
    (mpMainForm->cmbAreaId)->addItem(tr("Obihiro"),     QVariant(50));
    (mpMainForm->cmbAreaId)->addItem(tr("Kushiro"),     QVariant(51));
    (mpMainForm->cmbAreaId)->addItem(tr("Kitami"),      QVariant(49));
    (mpMainForm->cmbAreaId)->addItem(tr("Muroran"),     QVariant(53));
    (mpMainForm->cmbAreaId)->addItem(tr("Aomori"),      QVariant(2));
    (mpMainForm->cmbAreaId)->addItem(tr("Iwate"),       QVariant(3));
    (mpMainForm->cmbAreaId)->addItem(tr("Miyagi"),      QVariant(4));
    (mpMainForm->cmbAreaId)->addItem(tr("Akita"),       QVariant(5));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamagata"),    QVariant(6));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukushima"),   QVariant(7));
    (mpMainForm->cmbAreaId)->addItem(tr("Ibaraki"),     QVariant(8));
    (mpMainForm->cmbAreaId)->addItem(tr("Tochigi"),     QVariant(9));
    (mpMainForm->cmbAreaId)->addItem(tr("Gunma"),       QVariant(10));
    (mpMainForm->cmbAreaId)->addItem(tr("Saitama"),     QVariant(11));
    (mpMainForm->cmbAreaId)->addItem(tr("Chiba"),       QVariant(12));
    (mpMainForm->cmbAreaId)->addItem(tr("Tokyo"),       QVariant(13));
    (mpMainForm->cmbAreaId)->addItem(tr("Kanagawa"),    QVariant(14));
    (mpMainForm->cmbAreaId)->addItem(tr("Niigata"),     QVariant(15));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamanashi"),   QVariant(16));
    (mpMainForm->cmbAreaId)->addItem(tr("Nagano"),      QVariant(17));
    (mpMainForm->cmbAreaId)->addItem(tr("Toyama"),      QVariant(18));
    (mpMainForm->cmbAreaId)->addItem(tr("Ishikawa"),    QVariant(19));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukui"),       QVariant(20));
    (mpMainForm->cmbAreaId)->addItem(tr("Gifu"),        QVariant(21));
    (mpMainForm->cmbAreaId)->addItem(tr("Shizuoka"),    QVariant(22));
    (mpMainForm->cmbAreaId)->addItem(tr("Aichi"),       QVariant(23));
    (mpMainForm->cmbAreaId)->addItem(tr("Mie"),         QVariant(24));
    (mpMainForm->cmbAreaId)->addItem(tr("Shiga"),       QVariant(25));
    (mpMainForm->cmbAreaId)->addItem(tr("Kyoto"),       QVariant(26));
    (mpMainForm->cmbAreaId)->addItem(tr("Osaka"),       QVariant(27));
    (mpMainForm->cmbAreaId)->addItem(tr("Hyogo"),       QVariant(28));
    (mpMainForm->cmbAreaId)->addItem(tr("Nara"),        QVariant(29));
    (mpMainForm->cmbAreaId)->addItem(tr("Wakayama"),    QVariant(30));
    (mpMainForm->cmbAreaId)->addItem(tr("Tottori"),     QVariant(31));
    (mpMainForm->cmbAreaId)->addItem(tr("Shimane"),     QVariant(32));
    (mpMainForm->cmbAreaId)->addItem(tr("Okayama"),     QVariant(33));
    (mpMainForm->cmbAreaId)->addItem(tr("Hiroshima"),   QVariant(34));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamaguchi"),   QVariant(35));
    (mpMainForm->cmbAreaId)->addItem(tr("Tokushima"),   QVariant(36));
    (mpMainForm->cmbAreaId)->addItem(tr("Kagawa"),      QVariant(37));
    (mpMainForm->cmbAreaId)->addItem(tr("Ehime"),       QVariant(38));
    (mpMainForm->cmbAreaId)->addItem(tr("Kochi"),       QVariant(39));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukuoka"),     QVariant(40));
    (mpMainForm->cmbAreaId)->addItem(tr("Kitakyusyu"),  QVariant(54));
    (mpMainForm->cmbAreaId)->addItem(tr("Saga"),        QVariant(41));
    (mpMainForm->cmbAreaId)->addItem(tr("Nagasaki"),    QVariant(42));
    (mpMainForm->cmbAreaId)->addItem(tr("Kumamoto"),    QVariant(43));
    (mpMainForm->cmbAreaId)->addItem(tr("Ooita"),       QVariant(44));
    (mpMainForm->cmbAreaId)->addItem(tr("Miyazaki"),    QVariant(45));
    (mpMainForm->cmbAreaId)->addItem(tr("Kagoshima"),   QVariant(46));
    (mpMainForm->cmbAreaId)->addItem(tr("Okinawa"),     QVariant(47));

    // �n����Ƀ`�F�b�N������
    (mpMainForm->checkBox_0)->setCheckState(Qt::Checked);
    (mpMainForm->checkBox_1)->setCheckState(Qt::Checked);
    (mpMainForm->checkBox_2)->setCheckState(Qt::Checked);

    return;
}

/**
 *  �����̊J�n...
 */
void mainControl::startup(void)
{
    mpMainProcedures->startup();
    return;
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
