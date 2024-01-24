/**
 *   a2BCfavoriteBbsEditor
 * 
 *    [説明]
 *      お気に入り板編集クラス
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
 *  コンストラクタ
 * 
 */
a2BCfavoriteBbsEditor::a2BCfavoriteBbsEditor(QObject *parent)
 : QObject(parent)
{
}

/**
 *  デストラクタ
 * 
 */
a2BCfavoriteBbsEditor::~a2BCfavoriteBbsEditor()
{
    //
}


/**
 *  クラスの準備...
 * 
 * 
 */
void a2BCfavoriteBbsEditor::prepare(screenData *dbData, QTableView *tableViewWidget)
{
    mpDB                  = dbData;
    mpBbsTableTableWidget = tableViewWidget;
}

/**
 *  bbstable.htmlを読み込む
 * 
 */
bool a2BCfavoriteBbsEditor::readBbsTableInformations()
{
    // 板一覧データの読み込み準備
    QString fileName = mpDB->getBbsTableFileName();

    mBbsTableTrimmer.resetTrimmer();
    bool readyBbs = mBbsTableTrimmer.prepareTrimmer(fileName);
    if (readyBbs == false)
    {
        // 板一覧の解析失敗...終了する
        return (false);
    }

    // モデルを設定する
    mpBbsTableTableWidget->setModel(mBbsTableTrimmer.getTrimmerModel());

    QStringList tableHeader;
    tableHeader << tr("Board Name") << tr("Board Url") << tr("Sort Key");
    (mBbsTableTrimmer.getTrimmerModel())->setHorizontalHeaderLabels(tableHeader);

    QString message = tr("read :") + " " + fileName;
    emit updateMessage(message);

    return (true);
}

/**
 *  板テーブルの編集開始...
 * 
 */
void a2BCfavoriteBbsEditor::startBbsTableEditor()
{
    // テーブルウィジェットの情報を読み出す...
    QStandardItemModel *itemModel = mBbsTableTrimmer.getTrimmerModel();
    int count = itemModel->rowCount();
    if (count == 0)
    {
        // 取得済みのBBS情報を抽出する
        QString fileName = mpDB->getBbsTableFileName();
        mBbsTableTrimmer.readA2BLogBoards(fileName);
    }
    mpBbsTableTableWidget->resizeRowsToContents();    
    mpBbsTableTableWidget->resizeColumnsToContents();    
}

/**
 *  bbstable.htmlを保存する
 * 
 */
void a2BCfavoriteBbsEditor::saveBbsTableInformations()
{

/****
    // サポートされている codecを抽出してみる...
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
        // 何もせずに折り返す。。。
        return;
    }

    // テーブルウィジェットの情報を読み出して、設定して書き込む。
    QStandardItemModel *itemModel = mBbsTableTrimmer.getTrimmerModel();

    // テーブル情報をトリマ情報に直す...
    int count = itemModel->rowCount();
    for (int loop = 0; loop < count; loop++)
    {
        QModelIndex name = itemModel->index(loop, 0);
        QModelIndex url  = itemModel->index(loop, 1);
        QString boardData = (url.data()).toString() + ">" + (name.data()).toString();
        QByteArray outputData = boardData.toLocal8Bit();
        mBbsTableTrimmer.appendTrimmer(outputData);
    }    

    // bbstable.html を書き換える
    mBbsTableTrimmer.outputTrimmer();
    emit savedBbsEditor();

    // 書き換え処理の終了報告を行う
    QString message;
    message.setNum(count);
    message = tr("bbstable.html is updated.") + "\n\t"+ tr("count :") + " " + message;
    emit updateMessage(message);

    QMessageBox::information(0, tr("Saved bbstable.html"), message);
}

/**
 *  板情報を削除する
 * 
 */
void a2BCfavoriteBbsEditor::deleteBoardBbsInformation()
{
    // 選択行を抽出する
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
        // 削除対象行が存在しない...そのまま終了する
        return;
    }

    QString message = "'" + titles + " '" + tr("will be removed, are you ok?") + "\n";
    int reply = QMessageBox::question(0, tr("Confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (reply != QMessageBox::Yes)
    {
        return;
    }

    // 削除処理の実施
    for (int loop = indexList.count(); loop > 0; loop--)
    {
        itemModel->removeRows(indexList[loop - 1], 1, QModelIndex());
    }
    return;
}

/**
 *  板情報を追加する
 * 
 */
void a2BCfavoriteBbsEditor::appendBoardBbsInformation()
{
    // 板情報追加ダイアログを準備する
    QDialog            addDialog;
    Ui::addBoardDialog addBoardDialog;
    addBoardDialog.setupUi(&addDialog);
    
    // 板情報追加ダイアログをモーダルダイアログとして表示する
    addDialog.setModal(true);
    int retCode = addDialog.exec();
    if (retCode != QDialog::Accepted)
    {
        // OKボタンが押されずにキャンセルされた...終了する
        return;
    }

    // 入力された内容を(テーブル末尾に)登録する。
    QString boardName = (addBoardDialog.edtBoardName)->text();
    QString boardUrl  = (addBoardDialog.edtBoardUrl)->text();
    QString sortKey   = (addBoardDialog.edtSortKeyWord)->text();
    mBbsTableTrimmer.entryBbsData(boardName, boardUrl, sortKey);

    // 追加したことを画面表示
    QMessageBox::information(0, tr("Board Appended"), tr("data appended.") + "\n\n- - - - -\n" + tr("Name") + " : " + boardName + "\n" + tr("Url ") + " : " + boardUrl);
    return;
}

/**
 *  板情報を抽出する
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
