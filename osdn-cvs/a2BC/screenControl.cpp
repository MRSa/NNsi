/*!
 *   ��ʐ��䕔..
 *
 */
#include <QtGlobal>
#include <QMessageBox>
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QFileDialog>
#include "a2BCversion.h"
#include "screenControl.h"
#include "a2BCthread.h"

#include <QLocale>

/*!
 *  �R���X�g���N�^
 *
 */
screenControl::screenControl(Ui::a2BCMain *apForm, QString &execPath, QWidget *parent)
  : QWidget(parent), mpForm(apForm), mpDB(0), mpModel(0), mpHttpConn(0), mpHttpMain(0)
{
    createClasses(execPath);
    initializeDatas();
    connectSignals();
}

/*!
 *  �f�X�g���N�^
 *
 */
screenControl::~screenControl()
{
    deleteClasses();
}

/*!
 *  �N���X�̐���
 *
 */
void screenControl::createClasses(QString &execPath)
{
    mpDB          = new screenData();
    mpTimer       = new a2BCthread();
    mpHttpMain    = new QHttp();
    mpIndexParser = new a2BCindexParser();
    mpHttpConn    = new a2BChttpConnection(mpHttpMain);
    mpModel       = new a2BCmodel(mpDB, mpHttpConn, mpIndexParser);

    // �p�����[�^���t�@�C������ǂݏo��...
    mParameterLoader.loadFromFile(execPath, mpDB);

/**
    // �����ň�x�A�u�X���Ǘ��^�u�v������...
    (mpForm->manageBoard)->setVisible(false);
    (mpForm->syncInfoData)->setTabEnabled(3, false);
    (mpForm->syncInfoData)->removeTab(3);
**/

    return;
}

/*!
 *  �N���X�̍폜
 *
 */
void screenControl::deleteClasses(void)
{
/**
    if (mpDB != 0)
    {
        delete mpDB;
        mpDB = 0;
    }

    if (mpModel != 0)
    {
        delete mpModel;
        mpModel = 0;
    }
**/
}

/*!
 *  �f�[�^�̏�����(Window�f�[�^�̏�����)���s��
 *
 */
void screenControl::initializeDatas(void)
{
    // �X�e�[�^�X�o�[�Ƀv���O���X�o�[��ǉ�����
    mProgressBar.setGeometry(QRect(0, 0, 150, 16));
    (mpForm->statusbar)->addPermanentWidget(&mProgressBar);

    // �擾��URL
    QString data;
    data = mpDB->getBbsTableUrl();
    (mpForm->urlBbsTable)->setText(data);

    // �X�V�t�@�C����
    data = mpDB->getBbsTableFileName();
    (mpForm->lineEdit_File_bbsTable)->setText(data);

    // ���Q�����u���E�U�̊Ǘ��f�B���N�g��
    QString logDir = mpDB->getOtherBrowserDirectory();
    (mpForm->lineEdit_2chLogDirectory)->setText(logDir);

    // subject.txt���X�V����
    Qt::CheckState checkState;
    checkState = mpDB->getUpdateSubjectTxt();
    (mpForm->chkGetSubjectTxt)->setCheckState(checkState);

    // dat���������X�����ǂ߂�悤�ɂ���
    checkState = mpDB->getEnableDownThread();
    (mpForm->chkEnableDownThread)->setCheckState(checkState);

    // ���q�̃X�����Ǘ��Ώۂɂ���
    checkState = mpDB->getAddThreadToIndex();
    (mpForm->chkTrimDatLogFile)->setCheckState(checkState);
    (mpForm->chkTrimDatLogFile)->setEnabled(false);
    (mpForm->chkTrimDatLogFile)->hide();

    // �ꗗ�ɃX���擾���u���C�ɓ���v�Ƃ��Ēǉ�����
    checkState = mpDB->getAddToBbsTable();
    (mpForm->chkModifyBbsTable)->setCheckState(checkState);

    // �X����S�čĎ擾����
    checkState = mpDB->getDisableGetPart();
    (mpForm->chkGetAgainAll)->setCheckState(checkState);

    // �G���[�X�����Ǘ��Ώۂ���͂���
    checkState = mpDB->getRemoveErrorMessage();
    (mpForm->chkRemoveErrorThread)->setCheckState(checkState);

    // 1000�����X�����Ǘ��Ώۂ���͂���
    checkState = mpDB->getRemoveOverMessage();
    (mpForm->chkRemoveOverThread)->setCheckState(checkState);

    // �Ǘ��Ώۂł͂Ȃ����͕̂ʂQ�����u���E�U�Ɉړ�����
    checkState = mpDB->getBackupErrorLog();
    (mpForm->chkErrLogBackup)->setCheckState(checkState);

    // �X���ꗗ(subject.txt)���R�s�[����
    checkState = mpDB->getCopySubjectTxt();
    (mpForm->chkCopySubjectTxt)->setCheckState(checkState);

    // �X���擾�Ԋu
    QVariant interval = mpDB->getReceiveInterval();
    (mpForm->getInterval)->setText(interval.toString());

    // ���Q�����u���E�U�̃^�C�v
    switch (mpDB->getOtherBrowserType())
    {
      case OTHERBROWSER_GIKONAVI:
        (mpForm->gikoNavitype)->setChecked(true);
        break;
      case OTHERBROWSER_ABONE:
        (mpForm->Abonetype)->setChecked(true);
        break;
      case OTHERBROWSER_HZN2:
        (mpForm->hotZonu2type)->setChecked(true);
        break;
      case OTHERBROWSER_V2C:
        (mpForm->V2Ctype)->setChecked(true);
        break;
      case OTHERBROWSER_JANE:
        (mpForm->janeType)->setChecked(true);
        break;
      case OTHERBROWSER_PGIKO:
      default:
        (mpForm->pGIKOtype)->setChecked(true);
        break;
    }
    
    // ��������
    switch (mpDB->getSynchronizeType())
    {
      case SYNC_ALL:
        (mpForm->btnSynchronize)->setChecked(true);
        break;
      case SYNC_TO_a2B:
        (mpForm->btnToa2B)->setChecked(true);
        break;
      case SYNC_FROM_a2B:
      default:
        (mpForm->btnToOtherBrowser)->setChecked(true);
        break;        
    }

    // �{�^���Q�̗L��/������ݒ�ɂ���
    if (data.contains("bbstable.html") == false)
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(false);
        (mpForm->btnReadBoard)->setEnabled(false);
        (mpForm->executeUpdate)->setEnabled(false);
        (mpForm->executeSync)->setEnabled(false);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }
    else
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(true);
        (mpForm->btnReadBoard)->setEnabled(true);
        (mpForm->executeUpdate)->setEnabled(true);
        (mpForm->executeSync)->setEnabled(true);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }

//    (mpForm->btnAddBoard)->setVisible(false);
//    (mpForm->btnPickupBbs)->setVisible(false);
//    (mpForm->btnDeleteBoard)->setVisible(false);

    // �f�[�^�N���X�̏���
    mFavoriteEditor.prepare(mpDB, mpForm->tblViewFavorite);

    mFavoriteBbsEditor.prepare(mpDB, mpForm->BbsTableTableWidget);

    mpTimer->run();
    return;
}

/*!
 *  signal��slot��ڑ�����
 *
 */
void screenControl::connectSignals(void)
{
    qDebug("screenControl::connectSignals()");

    // "�Q��"�{�^�����������Ƃ��̏���
    connect(mpForm->pBtnSelectionBbsTable, SIGNAL(clicked()),
            this,                          SLOT(selection_bbsTableFile()));

    // "�Q��"�{�^�����������Ƃ��̏��� (���u���E�U�̊Ǘ��f�B���N�g���w��)
    connect(mpForm->pBtnSelectionOtherBrowserDirectory, SIGNAL(clicked()),
            this,                          SLOT(selection_otherBrowserDirectory()));

    // "bbstable.html�̍X�V"�{�^�����������Ƃ��̏���
    connect(mpForm->pBtnUpdateBbsTable, SIGNAL(clicked()),
            this,                       SLOT(select_updateBbsTableFile()));

    // "�ǂݏo��"�{�^�����������Ƃ��̏���
    connect(mpForm->btnReadBoard, SIGNAL(clicked()),
            this, SLOT(readBbsTableInformations()));

    // "�ۑ�"�{�^�����������Ƃ��̏���
    connect(mpForm->btnWriteBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(saveBbsTableInformations()));

    // "�ǉ�"�{�^�����������Ƃ��̏���
    connect(mpForm->btnAddBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(appendBoardBbsInformation()));

    // "�폜"�{�^�����������Ƃ��̏���
    connect(mpForm->btnDeleteBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(deleteBoardBbsInformation()));

    // "���o"�{�^�����������Ƃ��̏���
    connect(mpForm->btnPickupBbs, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(pickupBoardBbsInformation()));

    // �̕ҏW���I�������Ƃ��̏���
    connect(&mFavoriteBbsEditor, SIGNAL(savedBbsEditor()),
            this,               SLOT(savedBbsEditor()));

    // �\�����������b�Z�[�W�̎�M(��URL�Ǘ��p)
    connect(&mFavoriteBbsEditor, SIGNAL(updateMessage(const QString &)),
            this,               SLOT(updateMessage(const QString &)));

    // ���s�{�^�����������Ƃ��̏���
    connect(mpForm->executeUpdate, SIGNAL(clicked()),
            this,                  SLOT(executeUpdate()));

    // �������s�{�^�����������Ƃ��̏���
    connect(mpForm->executeSync, SIGNAL(clicked()),
            this,                  SLOT(executeSync()));

    // �L�����Z���{�^�������������̏���
    connect(mpForm->executeCancel, SIGNAL(clicked()),
            this,                  SLOT(cancelUpdate()));

    // �L�����Z���{�^�������������̏���
    connect(mpForm->executeCancel_2, SIGNAL(clicked()),
            this,                   SLOT(cancelUpdate()));

    // �e�L�X�g���X�V�����Ƃ��̏��� (�t�@�C����)
    connect(mpForm->lineEdit_File_bbsTable, SIGNAL(editingFinished()),
            this,                           SLOT(changed_bbsTableFile()));

    // �e�L�X�g���X�V�����Ƃ��̏��� (URL)
    connect(mpForm->urlBbsTable, SIGNAL(editingFinished()),
            this,                SLOT(changed_bbsTableUrl()));

    // �e�L�X�g���X�V�������̏���(�擾�Ԋu)
    connect(mpForm->getInterval, SIGNAL(editingFinished()),
            this,                SLOT(changed_interval()));

    // �\�����������b�Z�[�W�̎�M
    connect(mpModel, SIGNAL(updateMessage(const QString &)),
            this,    SLOT(updateMessage(const QString &)));

    // �\�����������b�Z�[�W�̎�M(�����p)
    connect(&mSyncLog, SIGNAL(updateMessage(const QString &)),
            this,     SLOT(updateMessage(const QString &)));

    // ���������̕�
    connect(&mSyncLog, SIGNAL(completedSynchronize(const QString &)),
            this,       SLOT(completedSynchronize(const QString &)));

    // ���C�ɓ���f�[�^�̓ǂݍ���
    connect(mpForm->btnReadFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,        SLOT(extractFavoriteData()));
    
    // ���C�ɓ���f�[�^�̏�������
    connect(mpForm->btnWriteFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,         SLOT(storeFavoriteData()));

    // ���C�ɓ���f�[�^�̍폜
    connect(mpForm->btnRemoveFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,          SLOT(removeFavoriteData()));

    // ���C�ɓ���f�[�^�̓���ւ�
    connect(mpForm->btnSwapFavorite,   SIGNAL(clicked()),
            &mFavoriteEditor,          SLOT(swapFavoriteData()));

    // �\�����������b�Z�[�W�̎�M(���C�ɓ���Ǘ��p)
    connect(&mFavoriteEditor, SIGNAL(updateMessage(const QString &)),
            this,            SLOT(updateMessage(const QString &)));

    // �^�C�}�ݒ�
    connect(mpModel, SIGNAL(setTimer(int, unsigned long)),
            mpTimer, SLOT(setTimer(int, unsigned long)));

    // �^�C���A�E�g�̎�M
    connect(mpTimer, SIGNAL(timeout(int)),
            mpModel, SLOT(receiveTimeout(int)));

    // ���O�擾�̊J�n
    connect(mpModel, SIGNAL(startedUpdate()),
            this,    SLOT(startedUpdate()));

    // ���O�擾�̏I��
    connect(mpModel, SIGNAL(completedUpdate()),
            this,    SLOT(completedUpdate()));

    // �ʐM��ԍX�V�̕�
    connect(mpHttpConn, SIGNAL(updateMessage(const QString &)),
            this,    SLOT(updateMessage(const QString &)));

    // �ʐM�I���̕�
    connect(mpHttpConn, SIGNAL(completedCommunication(int, QString &)),
            this,    SLOT(completedCommunication(int, QString &)));

    // �ʐM�I���̕�
    connect(this,    SIGNAL(completedCommunication(int)),
            mpModel,  SLOT(completedCommunication(int)));

    // ���̒ʐM�J�n...
    connect(this,    SIGNAL(nextCommunication()),
            mpModel,  SLOT(nextCommunication()));

    // �v���O���X�o�[�̍X�V
    connect(mpModel,        SIGNAL(updateValue(int)),
            &mProgressBar,  SLOT(setValue(int)));

    // �v���O���X�o�[�̍ő�l�ݒ�
    connect(mpModel,        SIGNAL(setMaximum(int)),
            &mProgressBar,  SLOT(setMaximum(int)));

    // �v���O���X�o�[�̍X�V
    connect(&mSyncLog,      SIGNAL(updateValue(int)),
            &mProgressBar,  SLOT(setValue(int)));

    // �v���O���X�o�[�̍ő�l�ݒ�
    connect(&mSyncLog,      SIGNAL(setMaximum(int)),
            &mProgressBar,  SLOT(setMaximum(int)));
        
    //
    //connect(mpHttpMain, SIGNAL(requestFinished(int, bool)),
            //mpHttpConn,   SLOT(requestFinished(int, bool)));

    // 
    connect(mpHttpMain, SIGNAL(done(bool)),
            mpHttpConn,   SLOT(done(bool)));

    // 
    connect(mpHttpMain, SIGNAL(readyRead(const QHttpResponseHeader &)),
            mpHttpConn,   SLOT(readyRead(const QHttpResponseHeader &)));

    // 
    connect(mpForm->actionAbout_Qt, SIGNAL(triggered(bool)),
            this,   SLOT(show_aboutQt(bool)));

    // 
    connect(mpForm->actionAbout_a2B, SIGNAL(triggered(bool)),
            this,   SLOT(show_about_a2BC(bool)));

    return;
}

/*!
 *  "�Q��..."�{�^�����������Ƃ��̏���
 *
 */
void screenControl::selection_bbsTableFile()
{
    qDebug("screenControl::selection_bbsTableFile()");

    QString fileName = mpDB->getBbsTableFileName();
    bool result = mpModel->selection_bbsTableFile(fileName);
    if (result == true)
    {
        // �w�肵���t�@�C������ݒ肷��
        (mpForm->lineEdit_File_bbsTable)->setText(fileName);
        mpDB->setBbsTableFileName(fileName);
    }
    if (fileName.contains("bbstable.html") == false)
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(false);
        (mpForm->btnReadBoard)->setEnabled(false);
        (mpForm->executeUpdate)->setEnabled(false);
        (mpForm->executeSync)->setEnabled(false);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }
    else
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(true);
        (mpForm->btnReadBoard)->setEnabled(true);
        (mpForm->executeUpdate)->setEnabled(true);
        (mpForm->executeSync)->setEnabled(true);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }

//    (mpForm->btnAddBoard)->setVisible(false);
//    (mpForm->btnPickupBbs)->setVisible(false);
//    (mpForm->btnDeleteBoard)->setVisible(false);

    return;
}

/*!
 *  "�Q��..."�{�^�����������Ƃ��̏��� (���u���E�U�f�B���N�g���̃f�B���N�g���w��)
 *
 */
void screenControl::selection_otherBrowserDirectory()
{
    qDebug("screenControl::selection_otherBrowserDirectory()");

    QString fileName = mpDB->getOtherBrowserDirectory();
    QString s = QFileDialog::getExistingDirectory(this, "", fileName);
    if (s == 0)
    {
        return;
    }

    // �w�肵���t�@�C������ݒ肷��
    (mpForm->lineEdit_2chLogDirectory)->setText(s);
    mpDB->setOtherBrowserDirectory(s);
    return;
}

/*!
 *  "bbstable.html�̍X�V" �{�^�����������Ƃ��̏���
 *
 */
void screenControl::select_updateBbsTableFile()
{
    // ��ʏ��������Ԃ֔��f������
    updateFormData();

    // �X�V����t�@�C�����̊m�F����
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // �t�@�C��������bbstable.html���w�肳��Ă��Ȃ������ꍇ
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // �擾����URL���m�F����
    QString url = mpDB->getBbsTableUrl();
    if (url.contains("bbstable.html") == false)
    {
        // URL����bbstable.html���w�肳��Ă��Ȃ������ꍇ
        QMessageBox mb("a2BC", tr("Invalid URL"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // bbstable.html�̍X�V�����s���邩�m�F����
    QString information = tr("Updates bbstable.html") + "\n    file name : " + fileName;
    information = information + "\n    " + "URL         : " + url + "\n\n";
    information = information + tr("Are you OK?");

    QMessageBox mb(tr("get bbstable.html"), information,
                   QMessageBox::Information,
                   QMessageBox::Ok | QMessageBox::Default,
                   QMessageBox::Cancel | QMessageBox::Escape,
                   QMessageBox::NoButton);
    mb.setButtonText(QMessageBox::Ok, tr("OK"));
    mb.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    switch(mb.exec())
    {
      case QMessageBox::Ok:
        // bbstable.html�̍X�V�����s����
        mpModel->update_bbsTableFile(fileName, url);
        break;

      case QMessageBox::Cancel:
      default:
        // ���������ɏI������
        break;
    }
    return;
}


/*!
 *   �u�������s�v�{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::executeSync()
{
    // ��ʏ��������Ԃ֔��f������
    updateFormData();

    // a2B�g�pbbstable.html�̃t�@�C�������o�^����Ă��邩�m�F����
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // �t�@�C��������bbstable.html���w�肳��Ă��Ȃ������ꍇ
        // (bbstable.html���w�肳��Ă��Ȃ����߁A���s�ł��Ȃ�)
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // ���O�����̏���...
    bool ret = mSyncLog.prepare(*mpDB);
    if (ret != true)
    {
        // �������s�I �񍐂��ďI������B
        QMessageBox mb("a2BC", tr("DO NOT READY FOR SYNC."),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // ���O������{���Ɏ��s���邩�m�F����
    QString information;
    information = "\n [" + mSyncLog.getSourceBrowserName() + " <==> " + mSyncLog.getDestinationBrowserName() + "] ";
    information = tr("Do synchronize, are you ready?") + information;
    QMessageBox mb(tr("sync a2B data"), information,
                   QMessageBox::Information,
                   QMessageBox::Ok | QMessageBox::Default,
                   QMessageBox::Cancel | QMessageBox::Escape,
                   QMessageBox::NoButton);
    mb.setButtonText(QMessageBox::Ok, tr("OK"));
    mb.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    switch(mb.exec())
    {
      case QMessageBox::Ok:
        // �L�����Z���{�^����L���ɂ���
        (mpForm->executeCancel)->setEnabled(true);
        (mpForm->executeCancel_2)->setEnabled(true);

        // ���O���������s����
        mSyncLog.doSync(mpDB->getSynchronizeType(), mpDB->getBackupErrorLog(), mpDB->getCopySubjectTxt());
        break;

      case QMessageBox::Cancel:
      default:
        // ���������ɏI������
        break;
    }

/**
    // �{�^���Q�̗L��/������ݒ�ɂ���
    (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
    (mpForm->pBtnUpdateBbsTable)->setEnabled(true);
    (mpForm->btnReadBoard)->setEnabled(true);
    (mpForm->executeUpdate)->setEnabled(true);
    (mpForm->executeSync)->setEnabled(true);
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);
**/
    return;
}

/*!
 *   �u���s�v�{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::executeUpdate()
{
    // ��ʏ��������Ԃ֔��f������
    updateFormData();

    // a2B�g�pbbstable.html�̃t�@�C�������o�^����Ă��邩�m�F����
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // �t�@�C��������bbstable.html���w�肳��Ă��Ȃ������ꍇ
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // �V���m�F�����s���� (�m�F���O�Ŏ��s�m�F����)
    mpModel->update_a2B_datas();

#if 0
    // �V���m�F�̍X�V�����s���邩�m�F����
    QString information = tr("Updates a2B datas, are you ready?");

    QMessageBox mb(tr("update a2B data"), information,
                   QMessageBox::Information,
                   QMessageBox::Ok | QMessageBox::Default,
                   QMessageBox::Cancel | QMessageBox::Escape,
                   QMessageBox::NoButton);
    mb.setButtonText(QMessageBox::Ok, tr("OK"));
    mb.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    switch(mb.exec())
    {
      case QMessageBox::Ok:
        // �L�����Z���{�^����L���ɂ���
        (mpForm->executeCancel)->setEnabled(true);
        (mpForm->executeCancel_2)->setEnabled(true);

        // �V���m�F�����s����
        mpModel->update_a2B_datas();
        break;

      case QMessageBox::Cancel:
      default:
        // ���������ɏI������
        break;
    }
#endif
    return;
}

/*!
 *   �u�L�����Z���v�{�^���������ꂽ�Ƃ��̏���
 *
 */
void screenControl::cancelUpdate()
{
    return;
}

/*!
 *   bbstable.html�̃t�@�C�������X�V���ꂽ�Ƃ�
 *
 */
void screenControl::changed_bbsTableFile()
{
    QString fileName = (mpForm->lineEdit_File_bbsTable)->text();
    mpDB->setBbsTableFileName(fileName);
    if (fileName.contains("bbstable.html") == false)
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(false);
        (mpForm->btnReadBoard)->setEnabled(false);
        (mpForm->executeUpdate)->setEnabled(false);
        (mpForm->executeSync)->setEnabled(false);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }
    else
    {
        // �{�^���Q�̗L��/������ݒ�ɂ���
        (mpForm->pBtnSelectionBbsTable)->setEnabled(true);
        (mpForm->pBtnUpdateBbsTable)->setEnabled(true);
        (mpForm->btnReadBoard)->setEnabled(true);
        (mpForm->executeUpdate)->setEnabled(true);
        (mpForm->executeSync)->setEnabled(true);
        (mpForm->executeCancel)->setEnabled(false);
        (mpForm->executeCancel_2)->setEnabled(false);
    }
    return;
}

/*!
 *   bbstable.html��URL�f�[�^���X�V���ꂽ�Ƃ�
 *
 */
void screenControl::changed_bbsTableUrl()
{
    mpDB->setBbsTableUrl((mpForm->urlBbsTable)->text());
    return;
}

/*!
 *   �擾�C���^�[�o���̒l���X�V���ꂽ�Ƃ�
 *
 */
void screenControl::changed_interval()
{
    QVariant interval = (mpForm->getInterval)->text();
    mpDB->setReceiveInterval(interval.toInt());

    interval = mpDB->getReceiveInterval();
    (mpForm->getInterval)->setText(interval.toString());

    return;
}

/*!
 *   ���b�Z�[�W�̕\���X�V
 *
 */
void screenControl::updateMessage(const QString &arMessage)
{
    QDateTime time = QDateTime::currentDateTime();
    QString message = time.toString("MM/dd hh:mm:ss") + " " + arMessage + "\n";
    (mpForm->informData)->insertPlainText(message);
    (mpForm->textBrowser)->insertPlainText(message);
    return;
}

/*!
 *   HTTP�ʐM�̏I��(�R�[���o�b�N)
 *
 */
void screenControl::completedCommunication(int error, QString &arMessage)
{
    // ���~�{�^���̖�����
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // ���b�Z�[�W�̕\��
    QString message;
    if (error < 0)
    {
        // �G���[�I������
        message = tr("Error Http Communication : ") + arMessage;
    }
    else
    {
        // ����I������
        message = tr("Completed Http Communication : ") + arMessage;
    }

    // ���b�Z�[�W�̍X�V...
    updateMessage(message);

    // �ʐM�����̃V�O�i�����s
    emit completedCommunication(error);

    return;
}


/*!
 *   ���O���������̏I��(�R�[���o�b�N)
 *
 */
void screenControl::completedSynchronize(const QString &arMessage)
{
    // ���s�{�^���̗L����
    (mpForm->executeUpdate)->setEnabled(true);
    (mpForm->executeSync)->setEnabled(true);

    // ���~�{�^���̖�����
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // ���b�Z�[�W�̍X�V...
    QString message = arMessage;
    updateMessage(message);

    // ���������̃V�O�i�����s
    emit completedSynchronize();

    // ���������̕\��...(���b�Z�[�W�{�b�N�X�ŕ\������)
    QString caption = tr("End Synchronize");
    QMessageBox::information(this, caption, message, QMessageBox::Ok);

    // �v���O���X�o�[�̏�����
    mProgressBar.reset();

    return;
}

/*!
 *   ��ʂ̃`�F�b�N��ԁAURL�A�t�@�C������������ɔ��f������
 *
 */
void screenControl::updateFormData(void)
{
    mpDB->setBbsTableUrl((mpForm->urlBbsTable)->text());
    mpDB->setBbsTableFileName((mpForm->lineEdit_File_bbsTable)->text());
    mpDB->setUpdateSubjectTxt((mpForm->chkGetSubjectTxt)->checkState());
    mpDB->setEnableDownThread((mpForm->chkEnableDownThread)->checkState());
    mpDB->setAddThreadToIndex((mpForm->chkTrimDatLogFile)->checkState());
    mpDB->setAddToBbsTable((mpForm->chkModifyBbsTable)->checkState());
    mpDB->setDisableGetPart((mpForm->chkGetAgainAll)->checkState());
    mpDB->setRemoveErrorMessage((mpForm->chkRemoveErrorThread)->checkState());
    mpDB->setRemoveOverMessage((mpForm->chkRemoveOverThread)->checkState());
    mpDB->setBackupErrorLog((mpForm->chkErrLogBackup)->checkState());
    mpDB->setBackupOverLog((mpForm->chkErrLogBackup)->checkState());
    mpDB->setCopySubjectTxt((mpForm->chkCopySubjectTxt)->checkState());

    mpDB->setOtherBrowserDirectory((mpForm->lineEdit_2chLogDirectory)->text());

    int otherBrowserType = OTHERBROWSER_PGIKO;
    if (((mpForm->gikoNavitype)->isChecked()) == true)
    {
        otherBrowserType = OTHERBROWSER_GIKONAVI;
    }
    if (((mpForm->Abonetype)->isChecked()) == true)
    {
        otherBrowserType = OTHERBROWSER_ABONE;
    }
    if (((mpForm->hotZonu2type)->isChecked()) == true)
    {
        otherBrowserType = OTHERBROWSER_HZN2;
    }
    if (((mpForm->V2Ctype)->isChecked()) == true)
    {
        otherBrowserType = OTHERBROWSER_V2C;
    }
    if (((mpForm->janeType)->isChecked()) == true)
    {
        otherBrowserType = OTHERBROWSER_JANE;
    }
    mpDB->setOtherBrowserType(otherBrowserType);


    int syncType = SYNC_ALL;
    if (((mpForm->btnToa2B)->isChecked()) == true)
    {
        syncType = SYNC_TO_a2B;
    }
    if (((mpForm->btnToOtherBrowser)->isChecked()) == true)
    {
        syncType = SYNC_FROM_a2B;
    }
    mpDB->setSynchronizeType(syncType);

    QVariant interval = (mpForm->getInterval)->text();
    mpDB->setReceiveInterval(interval.toInt());
    return;
}

/*!
 *   ���O�擾�J�n���̏���
 * 
 */
void screenControl::startedUpdate()
{
    // ���s�{�^���̖�����
    (mpForm->executeUpdate)->setEnabled(false);
    (mpForm->executeSync)->setEnabled(false);

    // ���~�{�^���̗L����
    (mpForm->executeCancel)->setEnabled(true);
    (mpForm->executeCancel_2)->setEnabled(false);
}

/*!
 *   ���O�擾�I�����̏���
 * 
 */
void screenControl::completedUpdate()
{
    // ���s�{�^���̗L����
    (mpForm->executeUpdate)->setEnabled(true);
    (mpForm->executeSync)->setEnabled(true);

    // ���~�{�^���̖�����
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // �v���O���X�o�[�̏�����
    mProgressBar.reset();
}

/*!
 *  �A�v���P�[�V�����I�����̏���
 * 
 */
void screenControl::finishApp()
{
    // �p�����[�^���f�[�^�ɓf���o��
    mParameterLoader.saveToFile();
    return;
}

/*!
 *  Qt�̃N���W�b�g��\������
 * 
 */
void screenControl::show_aboutQt(bool /* checked */)
{
    QMessageBox::aboutQt(this);
    return;
}

/*!
 *  a2BC�̃N���W�b�g��\������
 * 
 */
void screenControl::show_about_a2BC(bool /* checked */)
{
   QString caption = "about a2BC";
   QString message = A2BC_CREDIT + "\n" + A2BC_VERSION + " (" + A2BC_DATE + ")\n\n" + A2BC_COMMENT;

    QMessageBox::information(this, caption, message, QMessageBox::Ok);
    return;
}


/**
 *  bbstable.html ���X�V�I�������Ƃ��̏���...
 * 
 */
void screenControl::savedBbsEditor()
{
    // �{�^���͑S�Ė����ɂ���
    (mpForm->btnWriteBoard)->setEnabled(false);
    (mpForm->btnReadBoard)->setEnabled(false);
    (mpForm->btnPickupBbs)->setEnabled(false);

//     (mpForm->btnPickupBbs)->setVisible(false);
//     (mpForm->btnAddBoard)->setVisible(false);
//    (mpForm->btnDeleteBoard)->setVisible(false);

    (mpForm->btnAddBoard)->setEnabled(false);
    (mpForm->btnDeleteBoard)->setEnabled(false);

    // BBSTABLE�̍X�V�t���O�𗎂Ƃ��B
    (mpForm->chkModifyBbsTable)->setChecked(false);
    mpDB->setAddToBbsTable((mpForm->chkModifyBbsTable)->checkState());
}

/**
 *  bbstable.html��ǂݍ���
 * 
 */
void screenControl::readBbsTableInformations()
{
    // ��ʏ��������Ԃ֔��f������
    updateFormData();

    // a2B�g�pbbstable.html�̃t�@�C�������o�^����Ă��邩�m�F����
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // �t�@�C��������bbstable.html���w�肳��Ă��Ȃ������ꍇ
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    bool ret = mFavoriteBbsEditor.readBbsTableInformations();
    if (ret == true)
    {
        (mpForm->btnWriteBoard)->setEnabled(true);
        (mpForm->btnPickupBbs)->setEnabled(true);
/**/
//        (mpForm->btnPickupBbs)->setVisible(false);
//        (mpForm->btnAddBoard)->setVisible(false);
//        (mpForm->btnDeleteBoard)->setVisible(false);
/**/
        (mpForm->btnAddBoard)->setEnabled(true);
        (mpForm->btnDeleteBoard)->setEnabled(true);

        mFavoriteBbsEditor.startBbsTableEditor();
    }
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
