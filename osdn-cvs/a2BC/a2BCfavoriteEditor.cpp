/*!
 *   a2BCfavoriteEditor
 * 
 *    [����]
 *      ���C�ɓ���ҏW�N���X
 * 
 */
#include <QFile>
#include <QDateTime>
#include <QTextCodec>
#include <QTextStream>
#include <QStringList>
#include <QHeaderView>
#include <QTableView>
#include <QMessageBox>
#include <QItemSelectionModel>
#include "a2BCfavoriteEditor.h"
#include "a2BCfavoriteItemModel.h"
#include "a2BCfavoriteItemDelegate.h"
#include "screenData.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCfavoriteEditor::a2BCfavoriteEditor(QObject *parent)
 : mpItemView(0), mpHeaderView(0), mpItemModel(0), mpItemDelegate(0), mpDB(0)
{
    //
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BCfavoriteEditor::~a2BCfavoriteEditor()
{
    // �A�C�e�����f�����c���Ă����ꍇ...
    if (mpItemModel != 0)
    {
        delete mpItemModel;
    }
    if (mpItemDelegate != 0)
    {
        delete mpItemDelegate;
    }
}


/*!
 *  �N���X�̏���...
 * 
 * 
 */
void a2BCfavoriteEditor::prepare(screenData *apDB, QTableView *apView)
{
    mpDB       = apDB;
    mpItemView = apView;
}

/*!
 *  ���C�ɓ���f�[�^�̓ǂݍ��ݎw��
 * 
 * 
 */
void a2BCfavoriteEditor::extractFavoriteData()
{
    if (mpDB == 0)
    {
        return;
    }
    QString fileName = mpDB->getBbsTableFileName();
    fileName.replace(QRegExp("bbstable.html$"), "favorite.idx");
   
    // ���C�ɂ���o�^������������
    int count = mFavoriteParser.parseIndexFile(fileName);

    // �f�o�b�O�\��...
    QString message;
    message.setNum(count);
    message = tr("favorite file :") + " " + fileName + " (" + tr("count :") + message + ")";
    emit updateMessage(message);

    // item���f���N���X�̐؂�ւ�...
    a2BCfavoriteItemModel *newModel = new a2BCfavoriteItemModel(count, 12);
    mpItemView->setModel(newModel);
    if (mpItemModel != 0)
    {
        delete mpItemModel; 
    }
    mpItemModel = newModel;

    // delegate�N���X�̐؂�ւ�
    a2BCfavoriteItemDelegate *newDelegate = new a2BCfavoriteItemDelegate();
    mpItemView->setItemDelegate(newDelegate);
    if (mpItemDelegate != 0)
    {
        delete mpItemDelegate;
    }
    mpItemDelegate = newDelegate;

    for (int row = 0; row < count; row++)
    {
        a2BCfavoriteData *favoriteObject = mFavoriteParser.getFavoriteObject(row);
        if (favoriteObject != 0)
        {
            // ���C�ɓ���f�[�^�̃I�u�W�F�N�g���擾�ł����ꍇ...
            QString title = favoriteObject->getOption();
            QModelIndex ttlindex = mpItemModel->index(row, 0);
            mpItemModel->setData(ttlindex, title);
        
            int nofMsg = favoriteObject->getNofMessage();
            QModelIndex nofIndex = mpItemModel->index(row, 1);
            mpItemModel->setData(nofIndex, nofMsg);

            int curMsg = favoriteObject->getCurrentMessage();
            QModelIndex curIndex = mpItemModel->index(row, 2);
            mpItemModel->setData(curIndex, curMsg);

            QString fileName = favoriteObject->getDatFileName();
            QModelIndex fnIndex = mpItemModel->index(row, 3);
            mpItemModel->setData(fnIndex, fileName);

            QString url = favoriteObject->getUrl();
            QModelIndex urlIndex = mpItemModel->index(row, 4);
            mpItemModel->setData(urlIndex, url);

            int level = favoriteObject->getFavoriteLevel();
            QModelIndex levelIndex = mpItemModel->index(row, 5);
            mpItemModel->setData(levelIndex, level);

            quint64 lastUpdate = favoriteObject->getLastUpdate();
            uint last = (lastUpdate / (quint64) 1000);
            QDateTime dateTime;
            dateTime.setTime_t(last);
            QString updateDate = dateTime.toString("yyyy/MM/dd(ddd) hh:mm:ss");
            QModelIndex lastIndex = mpItemModel->index(row, 6);
            mpItemModel->setData(lastIndex, updateDate);
            
            QString nick = favoriteObject->getNickName();
            QModelIndex nickIndex = mpItemModel->index(row, 7);
            mpItemModel->setData(nickIndex, nick);
            
            int status = favoriteObject->getFileStatus();
            QModelIndex statusIndex = mpItemModel->index(row, 8);
            mpItemModel->setData(statusIndex, status);

            QModelIndex lastUIndex = mpItemModel->index(row, 9);
            mpItemModel->setData(lastUIndex, lastUpdate);

            int dummy2 = favoriteObject->getDummy2();
            QModelIndex dummy2Index = mpItemModel->index(row, 10);
            mpItemModel->setData(dummy2Index, dummy2);

            int dummy3 = favoriteObject->getDummy3();
            QModelIndex dummy3Index = mpItemModel->index(row, 11);
            mpItemModel->setData(dummy3Index, dummy3);
        }
    }

    if (mpItemView != 0)
    {
        QStringList tableHeader;
        tableHeader << tr("threadTitle") << tr("nofMsg") << tr("curMsg") << tr("datFileName") << tr("url") << tr("favorLevel") << tr("lastUpdate") << tr("nickName") << tr("msgStatus") << tr("lastUpdate") << tr("?dummy?");
        mpItemModel->setHeaderLabel(tableHeader);
        mpItemView->setModel(mpItemModel);
        mpItemView->resizeColumnToContents(0);
        mpItemView->resizeColumnToContents(1);
        mpItemView->hideColumn(2);
        mpItemView->hideColumn(3);
        mpItemView->resizeColumnToContents(4);
        mpItemView->resizeColumnToContents(5);
        mpItemView->hideColumn(6);
        mpItemView->resizeColumnToContents(7);
        mpItemView->hideColumn(8);        
        mpItemView->hideColumn(9);
        mpItemView->hideColumn(10);        
        mpItemView->hideColumn(11);
        mpItemView->setItemDelegate(mpItemDelegate);

        mpHeaderView = mpItemView->horizontalHeader();
        if (mpHeaderView != 0)
        {
            // �e�[�u���w�b�_���_�u���N���b�N������A�\�[�g�����s����
            connect(mpHeaderView, SIGNAL(sectionDoubleClicked(int)),
                    mpItemView,   SLOT(sortByColumn(int)));
           }
    }
/**
    QString tempStr;
    tempStr.setNum(count);
    QMessageBox::information(0, "a2BCfavoriteEditor::extractFavoriteData()", fileName + " count : " + tempStr, QMessageBox::Ok);
**/
}

/*!
 *  ���C�ɓ���f�[�^�̏������ݎw��
 * 
 * 
 * 
 */
void a2BCfavoriteEditor::storeFavoriteData()
{
    if (mpDB == 0)
    {
        return;
    }
    QString fileName = mpDB->getBbsTableFileName();
    fileName.replace(QRegExp("bbstable.html$"), "favorite.idx");
    int reply = QMessageBox::question(0, tr("write a2BCfavoriteEditor::storeFavoriteData()"), fileName + " " + tr("overwrites, are you OK?"),  QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (reply != QMessageBox::Yes)
    {
        return;
    }

    // �e�[�u���̃f�[�^���N���A����
    mFavoriteParser.clear();

    int entCount = 0;
    int count = mpItemModel->rowCount();
    for (int loop = 0; loop < count; loop++)
    {
        QString datFileName, url, nick, option;
        int maxMsg, curMsg, status, level, dummy2, dummy3, usable;
        quint64 lastUpdate;
        
        QModelIndex index = mpItemModel->index(loop, 0);
        option = (index.data()).toString();

        index =  mpItemModel->index(loop, 1);
        maxMsg = (index.data()).toInt();

        index =  mpItemModel->index(loop, 2);
        curMsg = (index.data()).toInt();

        index =  mpItemModel->index(loop, 3);
        datFileName = (index.data()).toString();

        index =  mpItemModel->index(loop, 4);
        url = (index.data()).toString();

        index =  mpItemModel->index(loop, 5);
        level = (index.data()).toInt();

        // �U�́i���������j�X�V���ԁi�Ȃ̂ŉ������Ȃ��j

        index =  mpItemModel->index(loop, 7);
        nick = (index.data()).toString();

        index =  mpItemModel->index(loop, 8);
        status = (index.data()).toInt();

        index =  mpItemModel->index(loop, 9);
        lastUpdate = (index.data()).toULongLong();

        index =  mpItemModel->index(loop, 10);
        dummy2 = (index.data()).toInt();

        index =  mpItemModel->index(loop, 11);
        dummy3 = (index.data()).toInt();
                    
        usable = 1;
        mFavoriteParser.appendData(fileName, datFileName, maxMsg, curMsg, status, level, lastUpdate, dummy2, dummy3, url, nick, option, usable);        
        entCount++;
    }

    // ���C�ɓ���t�@�C���������߂�
    mFavoriteParser.outputIndexFile(false, false);

    // �o�̓t�@�C������\������
    QString tempStr;
    tempStr.setNum(entCount);
    QMessageBox::information(0, tr("write done"), tempStr + tr("items are written."), QMessageBox::Ok);
    return;
}

/*!
 *  ���C�ɓ���f�[�^�̍폜
 *
 */
void a2BCfavoriteEditor::removeFavoriteData()
{
    QItemSelectionModel *selectionModel = mpItemView->selectionModel();
    int count    = mpItemModel->rowCount();
    int selCount = 0;
    QString titles = "";
    QVector<int> indexList;
    for (int loop = 0; loop < count; loop++)
    {
        if (selectionModel->isRowSelected(loop,  QModelIndex()) == true)
        {
            QModelIndex modelIndex = mpItemModel->index(loop, 0);
            selCount++;
            titles = titles + "   " + (mpItemModel->data(modelIndex)).toString() + "\n";
            indexList.append(loop);
        }
    }
    if (selCount == 0)
    {
        // �폜�Ώۍs�����݂��Ȃ�...���̂܂܏I������
        return;
    }
    QString countScreen;
    countScreen.setNum(selCount);

    QString message = countScreen + tr("items will be removed, are you ok?") + "\n\n" + titles;
    int reply = QMessageBox::question(0, tr("Confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (reply != QMessageBox::Yes)
    {
        return;
    }
    // �폜�����̎��{
    for (int loop = indexList.count(); loop > 0; loop--)
    {
        mpItemModel->removeRows(indexList[loop - 1], 1, QModelIndex());
    }
    return;
}

/*!
 *  ���C�ɓ���f�[�^�̓���ւ�
 *
 */
void a2BCfavoriteEditor::swapFavoriteData()
{
    QItemSelectionModel *selectionModel = mpItemView->selectionModel();
    int count    = mpItemModel->rowCount();
    int selCount = 0;
    QString titles = "";
    QVector<int> indexList;
    for (int loop = 0; loop < count; loop++)
    {
        if (selectionModel->isRowSelected(loop,  QModelIndex()) == true)
        {
            QModelIndex modelIndex = mpItemModel->index(loop, 0);
            selCount++;
            titles = titles + "   " + (mpItemModel->data(modelIndex)).toString() + "\n";
            indexList.append(loop);
           }
    }
    if (selCount != 2)
    {
        // ����ւ��Ώۂ�����ł��Ȃ�...���̂܂܏I������
        return;
       }

    // ����ւ������̎��{
    mpItemModel->myswapData(indexList[0], indexList[1]);
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
