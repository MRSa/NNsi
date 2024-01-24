/*!
 *   ����������..
 *
 */
#include <QCoreApplication>
#include <QUrl>
#include <QDir>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include "a2BCmodel.h"
#include "a2BCbbsTableTrim.h"

/*!
 *   �R���X�g���N�^
 *
 */
a2BCmodel::a2BCmodel(screenData *apDB, a2BChttpConnection *apHttp, a2BCindexParser *apParser, QWidget *parent)
 : QWidget(parent), mpDB(apDB), mpHttp(apHttp), mpIndexParser(apParser), mStartedGetting(-1),
   mGetUpdated(0), mGetFailure(0), mNotModified(0)
{
    initializeSelf();
}

/*!
 *    �f�X�g���N�^
 *
 */
a2BCmodel::~a2BCmodel()
{
    // �Ȃɂ����Ȃ�
}

/*!
 *  �N���X�̏�����
 *
 */
void a2BCmodel::initializeSelf(void)
{
    // �j�b�N�l�[�����N���A����
    mNick.clear();
    
    // ���X�g���N���A����
    for (QVector<a2BCgettingFile *>::ConstIterator it = mGettingFileList.begin(); it != mGettingFileList.end(); it++)
    {
        delete (*it);
    }
    mGettingFileList.clear();
    mGettingFileIndex.clear();
    resetTrimBbsTable();
    mFavoriteParser.clear();
    mDatFileChecker.resetData();
    return;
}

/*!
 *    bbstable.html�t�@�C����I������
 *
 */
bool a2BCmodel::selection_bbsTableFile(QString &arFileName)
{
    QString s = QFileDialog::getOpenFileName(this, "bbstable.html", arFileName, "bbstable.html");
    if (s == 0)
    {
        return (false);
    }
    arFileName = s;
    return (true);
}

/*!
 *    bbstable.html�t�@�C����URL����擾����
 *
 */
void a2BCmodel::update_bbsTableFile(QString &arFileName, QString &arUrl)
{
    QUrl url(arUrl);
    if (!url.isValid())
    {
        QMessageBox mb("a2BC", tr("Invalid URL : ") + arUrl,
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // �擾���C��...
    mpHttp->getFileFromUrl(arFileName, arUrl, false);
    return;
}

/*!
 *    bbstable.html�t�@�C���̎擾���L�����Z������
 *
 */
void a2BCmodel::cancelGetBbsTable()
{
    return;
}



/*!
 *  ���݂��� �f�B���N�g�����`�F�b�N����
 * 
 * 
 */
void a2BCmodel::checkDirectoryList(QDir &arBaseDirectory)
{
    QFileInfoList list = arBaseDirectory.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir() == true)
        {
            QString dirName = fileInfo.fileName();
            if (dirName.startsWith(".") != true)
            {
                // �X�V����f�B���N�g���������A�A�A
                mNick.append(dirName);
            }
        }
    }
    return;
}


/*!
 *   a2B�̃��O�f�B���N�g��������F������
 * 
 * 
 */
bool a2BCmodel::prepareA2BLogDirectory(bbsTableParser &arBbsTable, QString &arBaseDirectory)
{
    QString message, fileName;

    // �f�[�^������������
    initializeSelf();

    // �ꗗ�f�[�^�̓ǂݍ��ݏ���
    fileName = mpDB->getBbsTableFileName();
    bool readyBbs = arBbsTable.prepare(fileName);
    if (readyBbs == false)
    {
        // �ꗗ�̉�͎��s...�I������
        message = tr("bbs table read failure..."); 
        emit updateMessage(message);
        return (false);
    }
    int count = arBbsTable.numberOfBoards();
    message.setNum(count);
    message = tr("parsed board : ") + message;
    emit updateMessage(message);

    // �����p�f�B���N�g�������m�肷��
    arBaseDirectory = fileName;
    arBaseDirectory.replace(QRegExp("bbstable.html$"), "");

    // ���O�i�[�f�B���N�g���̈ꗗ���擾����
    QDir baseDir(arBaseDirectory);
    checkDirectoryList(baseDir);
    message = tr("Base directory : ") + arBaseDirectory; 
    emit updateMessage(message);

    /// ���C�ɓ���f�[�^��ǂݍ���...
    int favorCount = mFavoriteParser.parseIndexFile(arBaseDirectory + "favorite.idx");
    message.setNum(favorCount);
    message = tr(" favorite count : ") + message; 
    emit updateMessage(message);
    
    return (true);
}

/*
 *   a2b.idx�̃f�[�^���Ǘ�����
 * 
 * 
 */
int a2BCmodel::parseA2Bindex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick, int aOffset)
{
    ////// a2b.idx��ǂ�ŁA�擾����t�@�C���̈ꗗ���쐬���� /////
    QString boardUrl    = arBbsTable.getUrl(arBoardNick);
    if (boardUrl.isEmpty() == true)
    {
        // ��URL�̎擾�Ɏ��s�����Ƃ�...
        return (0);
    }
    QString targetIndex = arBaseDirectory + arBoardNick + "/a2b.idx";
    int count = mpIndexParser->parseIndexFile(targetIndex);

    ///// ���C�ɓ���f�[�^�ɓo�^����Ă���BBS��URL��bbstable.html �Ɛ������Ƃ�...
    ///// (���Ԃ������邾��[�ȁ[�[)
    mFavoriteParser.updateUrl(arBoardNick + "/", boardUrl);

    // �u���O�t�@�C�����Ď擾����v�̃`�F�b�N���
    Qt::CheckState getAll = mpDB->getDisableGetPart();

    for (int index = 0; index < count; index++)
    {
        bool append = true;
        a2BCgettingFile *datItem = new a2BCgettingFile();
        QString dataIndexFile    = mpIndexParser->getFileName(index + aOffset);
        QString datFileName      = arBaseDirectory + arBoardNick + "/" + dataIndexFile;
        datItem->setFileName(datFileName);
        int datStatus = mpIndexParser->getFileStatus(index + aOffset);
        if ((getAll == Qt::Checked)||((getAll == Qt::Unchecked)&&((datStatus == A2BCSTATUS_NOTYET)||(datStatus == A2BCSTATUS_RESERVE)||(datStatus == A2BCSTATUS_GETERROR))))
        {
            // �X�e�[�^�X�����擾�A�܂��́A�擾�\��A�G���[�������ꍇ�ɂ͑S���擾����
            datItem->setPartGet(false);
        }
        else
        {
            // �����擾�����{����
            datItem->setPartGet(true);
        }

        int dataNofMsg = mpIndexParser->getNofMessage(index + aOffset);
        if (dataNofMsg > 1000)
        {
            // ���X��1000�𒴂��Ă����X���͎擾���Ȃ�
            append = false;
        }

        QString datFileUrl = boardUrl + "dat/" + mpIndexParser->getFileName(index + aOffset);            
        datItem->setUrl(datFileUrl);
        if (append == true)
        {
            mGettingFileList.append(datItem);
            mGettingFileIndex.append((index + aOffset));
        }
    }

    // a2b.idx��ǂݏo�������ʂ�\������B
    QString message = "";
    message.setNum(count);
    message = "  " + targetIndex + " (" + message + ")";
//  emit updateMessage(message);

    return (count);
}

/*!
 *   �udat���������X�����ǂ߂�悤�ɂ���v�Ƀ`�F�b�N���ꂽ�Ƃ��̏���
 * 
 */
void a2BCmodel::parseOldThreadFile(QString &arBaseDirectory, QString &arBoardNick)
{
    // "dat���������X�����ǂ߂�悤�ɂ���" �ŁAdat�T���@�\��ǉ�...
    QDir targetDir(arBaseDirectory + arBoardNick);
    QStringList filter;
    filter << "*.dat";
    QStringList nameList = targetDir.entryList(filter);
    QString temp;
    int ct = 0;
    for (QStringList::const_iterator ite = nameList.constBegin(); ite != nameList.constEnd(); ite++)
    {
        // index�t�@�C���ɂȂ� dat�t�@�C���𔭌�����!!
        if (mpIndexParser->exists(*ite) < 0)
        {
            // �t�@�C���`�F�b�J�Ɋi�[����
            if (mDatFileChecker.appendDataFile((arBaseDirectory + arBoardNick), (*ite)) == true)
            {
                temp = temp + (*ite) + " ";
                ct++;
            }
        }
    }

    if (ct != 0)
    {
        // a2b.idx�ɒǉ������t�@�C��������Ε\������
        QString message;
        message.setNum(ct);
        temp = arBoardNick + " (" + message + ") : " + temp;
        emit updateMessage(temp);
    }
    return;
}

/*!
 *  subject.txt�������ɍX�V���邩�ǂ����̃`�F�b�N
 * 
 */
void a2BCmodel::parseSubjectTxtIndex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick)
{
    // subject.txt�������ɍX�V���邩�H
    Qt::CheckState checkState = mpDB->getUpdateSubjectTxt();
    if (checkState == Qt::Unchecked)
    {
        return;
    }
    QString subjectTxt = arBaseDirectory + arBoardNick + "/subject.txt";
    QString subjectUrl = arBbsTable.getUrl(arBoardNick) + "subject.txt";
    a2BCgettingFile *subjData = new a2BCgettingFile();
    subjData->setFileName(subjectTxt);
    subjData->setPartGet(false);
    subjData->setUrl(subjectUrl);
    mGettingFileList.append(subjData);
    mGettingFileIndex.append(-1);

    return;
}


/*!
 *  �X�V�����s���邩�m�F����
 * 
 */
bool a2BCmodel::confirmStartUpdate(void)
{
    QString message;
    message.setNum(mGettingFileList.size());
    message = tr(" start ") + tr("get threads: ") + message;
    QMessageBox mb(tr("get threads"), message,
                   QMessageBox::Information,
                   QMessageBox::Ok | QMessageBox::Default,
                   QMessageBox::Cancel | QMessageBox::Escape,
                   QMessageBox::NoButton);
    mb.setButtonText(QMessageBox::Ok, tr("OK"));
    mb.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    
    // �_�C�A���O�A�{�b�N�X�̊m�F
    if (mb.exec() != QMessageBox::Ok)
    {
        return (false);
    }
    return (true);
}

/*!
 *  a2B�̃f�[�^�X�V���C������...
 *
 */
void a2BCmodel::update_a2B_datas(void)
{
    bbsTableParser bbsTable;
    QString        a2BDirectory;

    // a2B�̃��O�f�B���N�g���֘A�̏���...
    if (prepareA2BLogDirectory(bbsTable, a2BDirectory) == false)
    {
        return;
    }

    // ���ׂĂɂ��Ď��s����...
    int offset = 0;
    for (QVector<QString>::ConstIterator it = mNick.begin(); it != mNick.end(); it++)
    {
        QString boardNick = (*it);
        
        // "a2b.idx"�ɂ��Ẳ�͏���
        offset = offset + parseA2Bindex(bbsTable, a2BDirectory, boardNick, offset);

        // "subject.txt"���X�V���邩�ǂ���...
        parseSubjectTxtIndex(bbsTable, a2BDirectory, boardNick);

        // �udat���������X����ǂ߂�悤�ɂ���v�Ƀ`�F�b�N�����邩�ǂ���...
        // (�udat���������X�����ǂ߂�悤�ɂ���v�Ƀ`�F�b�N����Ă��Ȃ��Ƃ��ɂ͂Ȃɂ����Ȃ�)
        if (mpDB->getEnableDownThread() != Qt::Unchecked)
        {
            parseOldThreadFile(a2BDirectory, boardNick);
        }
    }

    // ���O�擾�̊J�n��ʒm
    emit startedUpdate();

    // �擾���J�n���邩�ǂ������m�F����
    if (confirmStartUpdate() == false)
    {
        // ���������ɏI������
        mpIndexParser->clearIndexFile();

        // �I����ʒm
        emit completedUpdate();
        return;
    }

    // �擾���ʂ̏�����
    mGetUpdated  = 0;
    mGetFailure  = 0;
    mNotModified = 0;

    // bbstable.html�ɃX�����擾���邩�ǂ���
    if (mpDB->getAddToBbsTable() == Qt::Checked)
    {
        // bbstable.html�Ɏ擾�ς݃X����ǉ�����ꍇ...
        trimBbsTable(bbsTable);
    }

    // ����̃^�C���A�E�g�҂�
    unsigned long interval = mpDB->getReceiveInterval();
    mStartedGetting = 0;
    emit setTimer(0, interval);

    // �v���O���X�o�[�̍ő�l�ݒ�
    emit setMaximum(mGettingFileList.size() - 2);

    return;
}

/*!
 * 
 * 
 */
void a2BCmodel::resetTrimBbsTable(void)
{
    mBbsTableTrim.resetTrimmer();
}

/*!
 * 
 * 
 * 
 */
void a2BCmodel::trimBbsTable(bbsTableParser &arBbsTable)
{
    QString bbsTableFile = mpDB->getBbsTableFileName();
    QString message;

    message = tr("---");
    emit updateMessage(message);

    bool ret = mBbsTableTrim.prepareTrimmer(bbsTableFile);
    if (ret != true)
    {
        message = tr("modify bbstable.html failure");
        emit updateMessage(message);
        return;
    }

    for (QVector<QString>::ConstIterator it = mNick.begin(); it != mNick.end(); it++)
    {
        QString boardNick = (*it);
        QByteArray data = arBbsTable.getData(boardNick);
        if (data.isEmpty() == false)
        {
            // �o�^�f�[�^���i�[����
            mBbsTableTrim.appendTrimmer(data);
        }
    }
    
    ret = mBbsTableTrim.outputTrimmer();
    if (ret != true)
    {
        message = tr("modify bbstable.html failure");
        emit updateMessage(message);
        return;
    }
    message = tr("updated bbstable.html");
    emit updateMessage(message);
    return;
}

/*!
 *    bbstable.html�t�@�C���̎擾�I������
 *
 */
void a2BCmodel::finishGetBbsTable(void)
{
    return;
}

/*!
 *   �^�C���A�E�g�̎�M (subject.txt , ���X�̎擾���{)
 * 
 */
void a2BCmodel::receiveTimeout(int id)
{
    QString message = "";
  
    if (id >= mGettingFileList.size())
    {
        // �G���[�����������Ƃ��ɂ́A�Ǘ��Ώۂ���͂������H
        bool removeError = false;
        if (mpDB->getRemoveErrorMessage() == Qt::Checked)
        {
            removeError = true;
        }

        // 1000���������������Ƃ��ɂ́A�Ǘ��Ώۂ���͂������H
        bool removeOver = false;
        if (mpDB->getRemoveOverMessage() == Qt::Checked)
        {
            removeOver = true;
        }

        bool outputSubjectTxt = false;
        if (mpDB->getEnableDownThread() == Qt::Checked)
        {
            outputSubjectTxt = true;
        }
        mpIndexParser->outputIndexFile(removeError, removeOver, outputSubjectTxt, &mDatFileChecker);

        // ���C�ɓ���f�[�^���������ꍇ�ɂ́A�X�V����
        if (mFavoriteParser.count() > 0)
        {
            mFavoriteParser.outputIndexFile(removeError, removeOver);
            message = "\n---\n" + tr("updated favorite file");
            emit updateMessage(message);
        }

        // ���O�擾�̏I���B�B
        message.setNum(mStartedGetting);

        mStartedGetting = -1;
        message = "\n------\n" + tr("complete to get log :") + message + "\n------\n";
        emit updateMessage(message);

        // �I����ʒm����        
        emit completedUpdate();

        // �_�C�A���O�{�b�N�X�Ń��O�擾�̏I����ʒm����
        QString numTemp = "";
        numTemp.setNum(mGetUpdated);
        message = tr("completed to get log") + "\n   ";
        message = message + tr("updated : ") + numTemp  + "\n   ";
        numTemp = "";
        numTemp.setNum(mNotModified);
        message = message + tr("not modified : ") + numTemp  + "\n   ";
        numTemp = "";
        numTemp.setNum(mGetFailure);
        message = message + tr("error : ") + numTemp;
        QMessageBox::information(0, tr("a2BC"), message, QMessageBox::Ok);

        // �N���X���̃f�[�^���N���A����
        initializeSelf();
        return;
    }

    // ���Ɏ擾���郍�O�f�[�^���Ƃ��Ă���
    a2BCgettingFile *itemData = mGettingFileList[id];

    // ��؂��������
    message = "---";
    emit updateMessage(message);
 
    emit updateValue(id);
    
    // ���O�擾�����Ƃ��Ă���...
    QString fileName = itemData->getFileName();
    QString url      = itemData->getUrl();
    bool   partGet  = itemData->getPartGet();

    // �擾���@�̊m�F
    if (partGet == true)
    {
        // �����擾
        message = tr("update file: ");
    }
    else
    {
        // �S�擾
        message = tr("get file: ");
    }

    // �\��
    message = message + itemData->getFileName();
    emit updateMessage(message);
    
    // ���O�擾���Ăяo��
    mpHttp->getFileFromUrl(fileName, url, partGet);
 
    return;
}

/*!
 *   �ʐM�̏I��
 * 
 */
void a2BCmodel::completedCommunication(int error)
{
    // �V���m�F�����{���Ă��邩�`�F�b�N����
    if (mStartedGetting < 0)
    {
        // �V���m�F�����{...�Ȃ̂ŏI������
        return;
    }
    
    int status = A2BCSTATUS_UNKNOWN;
    int index = mGettingFileIndex[mStartedGetting];
    if (index >= 0)
    {
        // �ʐM���ʂ𔽉f������
        if (error > 0)
        {
            // �ʐM����
            status = A2BCSTATUS_UPDATE;
            mGetUpdated++;
        }
        else if (error == 0)
        {
            // �X�V�Ȃ��A�A��Ԃ͕ύX���Ȃ�
            status = A2BCSTATUS_UNKNOWN;
            mNotModified++;
        }
        else
        {
            // �ʐM���s�A�G���[��ԂɍX�V����
            status = A2BCSTATUS_GETERROR;
            mGetFailure++;
        }

        quint64 bbsTime = mpIndexParser->getLastUpdate(index);  // �X�V�O�̎������擾...
        mpIndexParser->setFileStatus(index, status);
        
        int dataNofMsg = mpIndexParser->getNofMessage(index);
        QString dataIndexFile = mpIndexParser->getFileName(index);
        QString boardNick = mpIndexParser->getNickName(index);

        /// ���C�ɓ���o�^�X�����ǂ����`�F�b�N���A�f�[�^�𓯊�������
        int favorIndex = mFavoriteParser.exists(boardNick, dataIndexFile);
        if (favorIndex >= 0)
        {
            // ���C�ɓ���Ȃ̂ŁA�}�[�N������
            mFavoriteParser.markIt(favorIndex);

            // �t�@�C����Ԃ��X�V����
            mFavoriteParser.setFileStatus(favorIndex, status);

            // ���C�ɓ���`�F�b�N
            QString message = tr("---");
            emit updateMessage(message);

            // ���C�ɓ���ɓo�^����Ă����X��������...
            quint64 favorTime = mFavoriteParser.getLastUpdate(favorIndex);
            int nofMsg       = mFavoriteParser.getNofMessage(favorIndex);
            if (favorTime > bbsTime)
            {
                // ���C�ɓ��� -> �ʏ�Ƀf�[�^���f
                int curMsg = mFavoriteParser.getCurrentMessage(favorIndex);
                mpIndexParser->setNofMessage(index, nofMsg);
                mpIndexParser->setCurrentMessage(index, curMsg);
                mpIndexParser->setFileStatus(index, status);
        
                message = tr("modify favorite -> board");
                emit updateMessage(message);
            }
            else
            {
                // �ʏ�� -> ���C�ɓ���Ƀf�[�^���f
                int curMsg = mpIndexParser->getCurrentMessage(index);
                mFavoriteParser.setNofMessage(favorIndex, dataNofMsg);
                mFavoriteParser.setCurrentMessage(favorIndex, curMsg);

                message = tr("modify board -> favorite");
                emit updateMessage(message);
            }
        }
    }

    // ���̃��O�擾�̂��߂ɃC�x���g�𔭍s����
    QEvent *event = new QEvent(QEvent::User);
    QCoreApplication::postEvent(this, event);
    return;
}

/*!
 *    ���̒ʐM�J�n...
 * 
 */
void a2BCmodel::nextCommunication()
{
    if (mStartedGetting < 0)
    {
        return;
    }

    unsigned long interval = mpDB->getReceiveInterval();
    mStartedGetting++;
    emit setTimer(mStartedGetting, interval);

    return;
}

/*!
 *  �C�x���g�̎�M
 * 
 */
void a2BCmodel::customEvent(QEvent *event)
{
    // null �Ȃ牽�����Ȃ�
    if (event == 0)
    {
        return;
    }
    
    // ���[�U��`�C�x���g��������A���̒ʐM���s��
    if (event->type() == QEvent::User)
    {
        nextCommunication();
    }
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
