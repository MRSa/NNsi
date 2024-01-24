/**
 *   a2BCfavoriteBbsEditor
 * 
 *    [����]
 *      ���C�ɓ���ҏW�N���X
 * 
 */
#include <QTextCodec>
#include <QString>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QDialog>
#include "screenControl.h"
#include "a2BCfavoriteBbsEditor.h"

/**
 *  �R���X�g���N�^
 * 
 */
a2BCfavoriteBbsEditor::a2BCfavoriteBbsEditor(QObject *parent)
 : QObject(parent)
{
}

/**
 *  �f�X�g���N�^
 * 
 */
a2BCfavoriteBbsEditor::~a2BCfavoriteBbsEditor()
{
    //
}


/**
 *  �N���X�̏���...
 * 
 * 
 */
void a2BCfavoriteBbsEditor::prepare(screenData *dbData, QTableView *tableViewWidget)
{
    mpDB                  = dbData;
    mpBbsTableTableWidget = tableViewWidget;
}

/**
 *  bbstable.html��ǂݍ���
 * 
 */
bool a2BCfavoriteBbsEditor::readBbsTableInformations()
{
    // �ꗗ�f�[�^�̓ǂݍ��ݏ���
    QString fileName = mpDB->getBbsTableFileName();

    mBbsTableTrimmer.resetTrimmer();
    bool readyBbs = mBbsTableTrimmer.prepareTrimmer(fileName);
    if (readyBbs == false)
    {
        // �ꗗ�̉�͎��s...�I������
        return (false);
    }

    // ���f����ݒ肷��
    mpBbsTableTableWidget->setModel(mBbsTableTrimmer.getTrimmerModel());

    QStringList tableHeader;
    tableHeader << tr("Board Name") << tr("Board Url") << tr("Sort Key");
    (mBbsTableTrimmer.getTrimmerModel())->setHorizontalHeaderLabels(tableHeader);

    QString message = tr("read :") + " " + fileName;
    emit updateMessage(message);

    return (true);
}

/**
 *  �e�[�u���̕ҏW�J�n...
 * 
 */
void a2BCfavoriteBbsEditor::startBbsTableEditor()
{
    // �e�[�u���E�B�W�F�b�g�̏���ǂݏo��...
    QStandardItemModel *itemModel = mBbsTableTrimmer.getTrimmerModel();
    int count = itemModel->rowCount();
    if (count == 0)
    {
        // �擾�ς݂�BBS���𒊏o����
        QString fileName = mpDB->getBbsTableFileName();
        mBbsTableTrimmer.readA2BLogBoards(fileName);
    }
    mpBbsTableTableWidget->resizeRowsToContents();    
    mpBbsTableTableWidget->resizeColumnsToContents();    
}

/**
 *  bbstable.html��ۑ�����
 * 
 */
void a2BCfavoriteBbsEditor::saveBbsTableInformations()
{

/****
    // �T�|�[�g����Ă��� codec�𒊏o���Ă݂�...
    QString msg = "";
    QList<QByteArray> codecLists;
    codecLists = QTextCodec::availableCodecs();
    for (int i = 0; i < codecLists.size(); ++i)
    {
        msg = msg + " " + codecLists.at(i);
    }
    QMessageBox::information(0, "codec list", msg);
****/

    if (QMessageBox::question(0, tr("Save bbstable.html"), tr("update favorite BBS, are you ready?"), ((QMessageBox::Ok)|(QMessageBox::Cancel))) != QMessageBox::Ok)
    {
        // ���������ɐ܂�Ԃ��B�B�B
        return;
    }

    // �e�[�u���E�B�W�F�b�g�̏���ǂݏo���āA�ݒ肵�ď������ށB
    QStandardItemModel *itemModel = mBbsTableTrimmer.getTrimmerModel();

    // �e�[�u�������g���}���ɒ���...
    int count = itemModel->rowCount();
    for (int loop = 0; loop < count; loop++)
    {
        QModelIndex name = itemModel->index(loop, 0);
        QModelIndex url  = itemModel->index(loop, 1);
        QString boardData = (url.data()).toString() + ">" + (name.data()).toString();
        QByteArray outputData = boardData.toLocal8Bit();
        mBbsTableTrimmer.appendTrimmer(outputData);
    }    

    // bbstable.html ������������
    mBbsTableTrimmer.outputTrimmer();
    emit savedBbsEditor();

    // �������������̏I���񍐂��s��
    QString message;
    message.setNum(count);
    message = tr("bbstable.html is updated.") + "\n\t"+ tr("count :") + " " + message;
    emit updateMessage(message);

    QMessageBox::information(0, tr("Saved bbstable.html"), message);
}

/**
 *  �����폜����
 * 
 */
void a2BCfavoriteBbsEditor::deleteBoardBbsInformation()
{
    // �I���s�𒊏o����
    QStandardItemModel  *itemModel      = mBbsTableTrimmer.getTrimmerModel();
    QItemSelectionModel *selectionModel = mpBbsTableTableWidget->selectionModel();
    int count     = itemModel->rowCount();
    int selCount  = 0;
    QString titles = "";
    QVector<int> indexList;
    for (int loop = 0; loop < count; loop++)
    {
        if (selectionModel->isRowSelected(loop,  QModelIndex()) == true)
        {
            QModelIndex modelIndex = itemModel->index(loop, 0);
            selCount++;
            titles = titles + " " + (itemModel->data(modelIndex)).toString();
            indexList.append(loop);
        }
    }
    if (selCount == 0)
    {
        // �폜�Ώۍs�����݂��Ȃ�...���̂܂܏I������
        return;
    }

    QString message = "'" + titles + " '" + tr("will be removed, are you ok?") + "\n";
    int reply = QMessageBox::question(0, tr("Confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (reply != QMessageBox::Yes)
    {
        return;
    }

    // �폜�����̎��{
    for (int loop = indexList.count(); loop > 0; loop--)
    {
        itemModel->removeRows(indexList[loop - 1], 1, QModelIndex());
    }
    return;
}

/**
 *  ����ǉ�����
 * 
 */
void a2BCfavoriteBbsEditor::appendBoardBbsInformation()
{
    // ���ǉ��_�C�A���O����������
    QDialog            addDialog;
    Ui::addBoardDialog addBoardDialog;
    addBoardDialog.setupUi(&addDialog);
    
    // ���ǉ��_�C�A���O�����[�_���_�C�A���O�Ƃ��ĕ\������
    addDialog.setModal(true);
    int retCode = addDialog.exec();
    if (retCode != QDialog::Accepted)
    {
        // OK�{�^���������ꂸ�ɃL�����Z�����ꂽ...�I������
        return;
    }

    // ���͂��ꂽ���e��(�e�[�u��������)�o�^����B
    QString boardName = (addBoardDialog.edtBoardName)->text();
    QString boardUrl  = (addBoardDialog.edtBoardUrl)->text();
    QString sortKey   = (addBoardDialog.edtSortKeyWord)->text();
    mBbsTableTrimmer.entryBbsData(boardName, boardUrl, sortKey);

    // �ǉ��������Ƃ���ʕ\��
    QMessageBox::information(0, tr("Board Appended"), tr("data appended.") + "\n\n- - - - -\n" + tr("Name") + " : " + boardName + "\n" + tr("Url ") + " : " + boardUrl);
    return;
}

/**
 *  ���𒊏o����
 * 
 */
void a2BCfavoriteBbsEditor::pickupBoardBbsInformation()
{
    mBbsTableTrimmer.clearLogBoardData();

    QStringList tableHeader;
    tableHeader << tr("Board Name") << tr("Board Url") << tr("Sort Key");
    (mBbsTableTrimmer.getTrimmerModel())->setHorizontalHeaderLabels(tableHeader);

    QString fileName = mpDB->getBbsTableFileName();
    mBbsTableTrimmer.readA2BLogBoards(fileName);

    mpBbsTableTableWidget->resizeRowsToContents();    
    mpBbsTableTableWidget->resizeColumnsToContents();    
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
