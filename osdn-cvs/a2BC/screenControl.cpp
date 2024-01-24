/*!
 *   画面制御部..
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
 *  コンストラクタ
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
 *  デストラクタ
 *
 */
screenControl::~screenControl()
{
    deleteClasses();
}

/*!
 *  クラスの生成
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

    // パラメータをファイルから読み出す...
    mParameterLoader.loadFromFile(execPath, mpDB);

/**
    // ここで一度、「スレ管理タブ」を消す...
    (mpForm->manageBoard)->setVisible(false);
    (mpForm->syncInfoData)->setTabEnabled(3, false);
    (mpForm->syncInfoData)->removeTab(3);
**/

    return;
}

/*!
 *  クラスの削除
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
 *  データの初期化(Windowデータの初期化)を行う
 *
 */
void screenControl::initializeDatas(void)
{
    // ステータスバーにプログレスバーを追加する
    mProgressBar.setGeometry(QRect(0, 0, 150, 16));
    (mpForm->statusbar)->addPermanentWidget(&mProgressBar);

    // 取得先URL
    QString data;
    data = mpDB->getBbsTableUrl();
    (mpForm->urlBbsTable)->setText(data);

    // 更新ファイル名
    data = mpDB->getBbsTableFileName();
    (mpForm->lineEdit_File_bbsTable)->setText(data);

    // 他２ちゃんブラウザの管理ディレクトリ
    QString logDir = mpDB->getOtherBrowserDirectory();
    (mpForm->lineEdit_2chLogDirectory)->setText(logDir);

    // subject.txtも更新する
    Qt::CheckState checkState;
    checkState = mpDB->getUpdateSubjectTxt();
    (mpForm->chkGetSubjectTxt)->setCheckState(checkState);

    // dat落ちしたスレも読めるようにする
    checkState = mpDB->getEnableDownThread();
    (mpForm->chkEnableDownThread)->setCheckState(checkState);

    // 迷子のスレを管理対象にする
    checkState = mpDB->getAddThreadToIndex();
    (mpForm->chkTrimDatLogFile)->setCheckState(checkState);
    (mpForm->chkTrimDatLogFile)->setEnabled(false);
    (mpForm->chkTrimDatLogFile)->hide();

    // 板一覧にスレ取得板を「お気に入り板」として追加する
    checkState = mpDB->getAddToBbsTable();
    (mpForm->chkModifyBbsTable)->setCheckState(checkState);

    // スレを全て再取得する
    checkState = mpDB->getDisableGetPart();
    (mpForm->chkGetAgainAll)->setCheckState(checkState);

    // エラースレを管理対象からはずす
    checkState = mpDB->getRemoveErrorMessage();
    (mpForm->chkRemoveErrorThread)->setCheckState(checkState);

    // 1000超えスレを管理対象からはずす
    checkState = mpDB->getRemoveOverMessage();
    (mpForm->chkRemoveOverThread)->setCheckState(checkState);

    // 管理対象ではないものは別２ちゃんブラウザに移動する
    checkState = mpDB->getBackupErrorLog();
    (mpForm->chkErrLogBackup)->setCheckState(checkState);

    // スレ一覧(subject.txt)もコピーする
    checkState = mpDB->getCopySubjectTxt();
    (mpForm->chkCopySubjectTxt)->setCheckState(checkState);

    // スレ取得間隔
    QVariant interval = mpDB->getReceiveInterval();
    (mpForm->getInterval)->setText(interval.toString());

    // 他２ちゃんブラウザのタイプ
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
    
    // 同期方式
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

    // ボタン群の有効/無効を設定にする
    if (data.contains("bbstable.html") == false)
    {
        // ボタン群の有効/無効を設定にする
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
        // ボタン群の有効/無効を設定にする
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

    // データクラスの準備
    mFavoriteEditor.prepare(mpDB, mpForm->tblViewFavorite);

    mFavoriteBbsEditor.prepare(mpDB, mpForm->BbsTableTableWidget);

    mpTimer->run();
    return;
}

/*!
 *  signalとslotを接続する
 *
 */
void screenControl::connectSignals(void)
{
    qDebug("screenControl::connectSignals()");

    // "参照"ボタンを押したときの処理
    connect(mpForm->pBtnSelectionBbsTable, SIGNAL(clicked()),
            this,                          SLOT(selection_bbsTableFile()));

    // "参照"ボタンを押したときの処理 (他ブラウザの管理ディレクトリ指定)
    connect(mpForm->pBtnSelectionOtherBrowserDirectory, SIGNAL(clicked()),
            this,                          SLOT(selection_otherBrowserDirectory()));

    // "bbstable.htmlの更新"ボタンを押したときの処理
    connect(mpForm->pBtnUpdateBbsTable, SIGNAL(clicked()),
            this,                       SLOT(select_updateBbsTableFile()));

    // "板読み出し"ボタンを押したときの処理
    connect(mpForm->btnReadBoard, SIGNAL(clicked()),
            this, SLOT(readBbsTableInformations()));

    // "板保存"ボタンを押したときの処理
    connect(mpForm->btnWriteBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(saveBbsTableInformations()));

    // "板追加"ボタンを押したときの処理
    connect(mpForm->btnAddBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(appendBoardBbsInformation()));

    // "板削除"ボタンを押したときの処理
    connect(mpForm->btnDeleteBoard, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(deleteBoardBbsInformation()));

    // "板抽出"ボタンを押したときの処理
    connect(mpForm->btnPickupBbs, SIGNAL(clicked()),
            &mFavoriteBbsEditor, SLOT(pickupBoardBbsInformation()));

    // 板の編集が終了したときの処理
    connect(&mFavoriteBbsEditor, SIGNAL(savedBbsEditor()),
            this,               SLOT(savedBbsEditor()));

    // 表示したいメッセージの受信(板URL管理用)
    connect(&mFavoriteBbsEditor, SIGNAL(updateMessage(const QString &)),
            this,               SLOT(updateMessage(const QString &)));

    // 実行ボタンを押したときの処理
    connect(mpForm->executeUpdate, SIGNAL(clicked()),
            this,                  SLOT(executeUpdate()));

    // 同期実行ボタンを押したときの処理
    connect(mpForm->executeSync, SIGNAL(clicked()),
            this,                  SLOT(executeSync()));

    // キャンセルボタンを押した時の処理
    connect(mpForm->executeCancel, SIGNAL(clicked()),
            this,                  SLOT(cancelUpdate()));

    // キャンセルボタンを押した時の処理
    connect(mpForm->executeCancel_2, SIGNAL(clicked()),
            this,                   SLOT(cancelUpdate()));

    // テキストを更新したときの処理 (ファイル名)
    connect(mpForm->lineEdit_File_bbsTable, SIGNAL(editingFinished()),
            this,                           SLOT(changed_bbsTableFile()));

    // テキストを更新したときの処理 (URL)
    connect(mpForm->urlBbsTable, SIGNAL(editingFinished()),
            this,                SLOT(changed_bbsTableUrl()));

    // テキストを更新した時の処理(取得間隔)
    connect(mpForm->getInterval, SIGNAL(editingFinished()),
            this,                SLOT(changed_interval()));

    // 表示したいメッセージの受信
    connect(mpModel, SIGNAL(updateMessage(const QString &)),
            this,    SLOT(updateMessage(const QString &)));

    // 表示したいメッセージの受信(同期用)
    connect(&mSyncLog, SIGNAL(updateMessage(const QString &)),
            this,     SLOT(updateMessage(const QString &)));

    // 同期完了の報告
    connect(&mSyncLog, SIGNAL(completedSynchronize(const QString &)),
            this,       SLOT(completedSynchronize(const QString &)));

    // お気に入りデータの読み込み
    connect(mpForm->btnReadFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,        SLOT(extractFavoriteData()));
    
    // お気に入りデータの書き込み
    connect(mpForm->btnWriteFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,         SLOT(storeFavoriteData()));

    // お気に入りデータの削除
    connect(mpForm->btnRemoveFavorite, SIGNAL(clicked()),
            &mFavoriteEditor,          SLOT(removeFavoriteData()));

    // お気に入りデータの入れ替え
    connect(mpForm->btnSwapFavorite,   SIGNAL(clicked()),
            &mFavoriteEditor,          SLOT(swapFavoriteData()));

    // 表示したいメッセージの受信(お気に入り管理用)
    connect(&mFavoriteEditor, SIGNAL(updateMessage(const QString &)),
            this,            SLOT(updateMessage(const QString &)));

    // タイマ設定
    connect(mpModel, SIGNAL(setTimer(int, unsigned long)),
            mpTimer, SLOT(setTimer(int, unsigned long)));

    // タイムアウトの受信
    connect(mpTimer, SIGNAL(timeout(int)),
            mpModel, SLOT(receiveTimeout(int)));

    // ログ取得の開始
    connect(mpModel, SIGNAL(startedUpdate()),
            this,    SLOT(startedUpdate()));

    // ログ取得の終了
    connect(mpModel, SIGNAL(completedUpdate()),
            this,    SLOT(completedUpdate()));

    // 通信状態更新の報告
    connect(mpHttpConn, SIGNAL(updateMessage(const QString &)),
            this,    SLOT(updateMessage(const QString &)));

    // 通信終了の報告
    connect(mpHttpConn, SIGNAL(completedCommunication(int, QString &)),
            this,    SLOT(completedCommunication(int, QString &)));

    // 通信終了の報告
    connect(this,    SIGNAL(completedCommunication(int)),
            mpModel,  SLOT(completedCommunication(int)));

    // 次の通信開始...
    connect(this,    SIGNAL(nextCommunication()),
            mpModel,  SLOT(nextCommunication()));

    // プログレスバーの更新
    connect(mpModel,        SIGNAL(updateValue(int)),
            &mProgressBar,  SLOT(setValue(int)));

    // プログレスバーの最大値設定
    connect(mpModel,        SIGNAL(setMaximum(int)),
            &mProgressBar,  SLOT(setMaximum(int)));

    // プログレスバーの更新
    connect(&mSyncLog,      SIGNAL(updateValue(int)),
            &mProgressBar,  SLOT(setValue(int)));

    // プログレスバーの最大値設定
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
 *  "参照..."ボタンを押したときの処理
 *
 */
void screenControl::selection_bbsTableFile()
{
    qDebug("screenControl::selection_bbsTableFile()");

    QString fileName = mpDB->getBbsTableFileName();
    bool result = mpModel->selection_bbsTableFile(fileName);
    if (result == true)
    {
        // 指定したファイル名を設定する
        (mpForm->lineEdit_File_bbsTable)->setText(fileName);
        mpDB->setBbsTableFileName(fileName);
    }
    if (fileName.contains("bbstable.html") == false)
    {
        // ボタン群の有効/無効を設定にする
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
        // ボタン群の有効/無効を設定にする
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
 *  "参照..."ボタンを押したときの処理 (他ブラウザディレクトリのディレクトリ指定)
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

    // 指定したファイル名を設定する
    (mpForm->lineEdit_2chLogDirectory)->setText(s);
    mpDB->setOtherBrowserDirectory(s);
    return;
}

/*!
 *  "bbstable.htmlの更新" ボタンを押したときの処理
 *
 */
void screenControl::select_updateBbsTableFile()
{
    // 画面情報を内部状態へ反映させる
    updateFormData();

    // 更新するファイル名の確認する
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // ファイル名欄にbbstable.htmlが指定されていなかった場合
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // 取得するURLを確認する
    QString url = mpDB->getBbsTableUrl();
    if (url.contains("bbstable.html") == false)
    {
        // URL欄にbbstable.htmlが指定されていなかった場合
        QMessageBox mb("a2BC", tr("Invalid URL"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // bbstable.htmlの更新を実行するか確認する
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
        // bbstable.htmlの更新を実行する
        mpModel->update_bbsTableFile(fileName, url);
        break;

      case QMessageBox::Cancel:
      default:
        // 何もせずに終了する
        break;
    }
    return;
}


/*!
 *   「同期実行」ボタンが押されたときの処理
 *
 */
void screenControl::executeSync()
{
    // 画面情報を内部状態へ反映させる
    updateFormData();

    // a2B使用bbstable.htmlのファイル名が登録されているか確認する
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // ファイル名欄にbbstable.htmlが指定されていなかった場合
        // (bbstable.htmlが指定されていないため、実行できない)
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // ログ同期の準備...
    bool ret = mSyncLog.prepare(*mpDB);
    if (ret != true)
    {
        // 準備失敗！ 報告して終了する。
        QMessageBox mb("a2BC", tr("DO NOT READY FOR SYNC."),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // ログ同期を本当に実行するか確認する
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
        // キャンセルボタンを有効にする
        (mpForm->executeCancel)->setEnabled(true);
        (mpForm->executeCancel_2)->setEnabled(true);

        // ログ同期を実行する
        mSyncLog.doSync(mpDB->getSynchronizeType(), mpDB->getBackupErrorLog(), mpDB->getCopySubjectTxt());
        break;

      case QMessageBox::Cancel:
      default:
        // 何もせずに終了する
        break;
    }

/**
    // ボタン群の有効/無効を設定にする
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
 *   「実行」ボタンが押されたときの処理
 *
 */
void screenControl::executeUpdate()
{
    // 画面情報を内部状態へ反映させる
    updateFormData();

    // a2B使用bbstable.htmlのファイル名が登録されているか確認する
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // ファイル名欄にbbstable.htmlが指定されていなかった場合
        QMessageBox mb("a2BC", tr("Invalid FileName"),
                       QMessageBox::Information,
                       QMessageBox::Ok | QMessageBox::Default,
                       QMessageBox::NoButton,
                       QMessageBox::NoButton);
        mb.exec();
        return;
    }

    // 新着確認を実行する (確認直前で実行確認する)
    mpModel->update_a2B_datas();

#if 0
    // 新着確認の更新を実行するか確認する
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
        // キャンセルボタンを有効にする
        (mpForm->executeCancel)->setEnabled(true);
        (mpForm->executeCancel_2)->setEnabled(true);

        // 新着確認を実行する
        mpModel->update_a2B_datas();
        break;

      case QMessageBox::Cancel:
      default:
        // 何もせずに終了する
        break;
    }
#endif
    return;
}

/*!
 *   「キャンセル」ボタンが押されたときの処理
 *
 */
void screenControl::cancelUpdate()
{
    return;
}

/*!
 *   bbstable.htmlのファイル名が更新されたとき
 *
 */
void screenControl::changed_bbsTableFile()
{
    QString fileName = (mpForm->lineEdit_File_bbsTable)->text();
    mpDB->setBbsTableFileName(fileName);
    if (fileName.contains("bbstable.html") == false)
    {
        // ボタン群の有効/無効を設定にする
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
        // ボタン群の有効/無効を設定にする
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
 *   bbstable.htmlのURLデータが更新されたとき
 *
 */
void screenControl::changed_bbsTableUrl()
{
    mpDB->setBbsTableUrl((mpForm->urlBbsTable)->text());
    return;
}

/*!
 *   取得インターバルの値が更新されたとき
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
 *   メッセージの表示更新
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
 *   HTTP通信の終了(コールバック)
 *
 */
void screenControl::completedCommunication(int error, QString &arMessage)
{
    // 中止ボタンの無効化
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // メッセージの表示
    QString message;
    if (error < 0)
    {
        // エラー終了した
        message = tr("Error Http Communication : ") + arMessage;
    }
    else
    {
        // 正常終了した
        message = tr("Completed Http Communication : ") + arMessage;
    }

    // メッセージの更新...
    updateMessage(message);

    // 通信完了のシグナル発行
    emit completedCommunication(error);

    return;
}


/*!
 *   ログ同期処理の終了(コールバック)
 *
 */
void screenControl::completedSynchronize(const QString &arMessage)
{
    // 実行ボタンの有効化
    (mpForm->executeUpdate)->setEnabled(true);
    (mpForm->executeSync)->setEnabled(true);

    // 中止ボタンの無効化
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // メッセージの更新...
    QString message = arMessage;
    updateMessage(message);

    // 同期完了のシグナル発行
    emit completedSynchronize();

    // 同期完了の表示...(メッセージボックスで表示する)
    QString caption = tr("End Synchronize");
    QMessageBox::information(this, caption, message, QMessageBox::Ok);

    // プログレスバーの初期化
    mProgressBar.reset();

    return;
}

/*!
 *   画面のチェック状態、URL、ファイル情報を内部情報に反映させる
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
 *   ログ取得開始時の処理
 * 
 */
void screenControl::startedUpdate()
{
    // 実行ボタンの無効化
    (mpForm->executeUpdate)->setEnabled(false);
    (mpForm->executeSync)->setEnabled(false);

    // 中止ボタンの有効化
    (mpForm->executeCancel)->setEnabled(true);
    (mpForm->executeCancel_2)->setEnabled(false);
}

/*!
 *   ログ取得終了時の処理
 * 
 */
void screenControl::completedUpdate()
{
    // 実行ボタンの有効化
    (mpForm->executeUpdate)->setEnabled(true);
    (mpForm->executeSync)->setEnabled(true);

    // 中止ボタンの無効化
    (mpForm->executeCancel)->setEnabled(false);
    (mpForm->executeCancel_2)->setEnabled(false);

    // プログレスバーの初期化
    mProgressBar.reset();
}

/*!
 *  アプリケーション終了時の処理
 * 
 */
void screenControl::finishApp()
{
    // パラメータをデータに吐き出す
    mParameterLoader.saveToFile();
    return;
}

/*!
 *  Qtのクレジットを表示する
 * 
 */
void screenControl::show_aboutQt(bool /* checked */)
{
    QMessageBox::aboutQt(this);
    return;
}

/*!
 *  a2BCのクレジットを表示する
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
 *  bbstable.html を更新終了したときの処理...
 * 
 */
void screenControl::savedBbsEditor()
{
    // ボタンは全て無効にする
    (mpForm->btnWriteBoard)->setEnabled(false);
    (mpForm->btnReadBoard)->setEnabled(false);
    (mpForm->btnPickupBbs)->setEnabled(false);

//     (mpForm->btnPickupBbs)->setVisible(false);
//     (mpForm->btnAddBoard)->setVisible(false);
//    (mpForm->btnDeleteBoard)->setVisible(false);

    (mpForm->btnAddBoard)->setEnabled(false);
    (mpForm->btnDeleteBoard)->setEnabled(false);

    // BBSTABLEの更新フラグを落とす。
    (mpForm->chkModifyBbsTable)->setChecked(false);
    mpDB->setAddToBbsTable((mpForm->chkModifyBbsTable)->checkState());
}

/**
 *  bbstable.htmlを読み込む
 * 
 */
void screenControl::readBbsTableInformations()
{
    // 画面情報を内部状態へ反映させる
    updateFormData();

    // a2B使用bbstable.htmlのファイル名が登録されているか確認する
    QString fileName = mpDB->getBbsTableFileName();
    if (fileName.contains("bbstable.html") == false)
    {
        // ファイル名欄にbbstable.htmlが指定されていなかった場合
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
