/*!
 *   処理実装部..
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
 *   コンストラクタ
 *
 */
a2BCmodel::a2BCmodel(screenData *apDB, a2BChttpConnection *apHttp, a2BCindexParser *apParser, QWidget *parent)
 : QWidget(parent), mpDB(apDB), mpHttp(apHttp), mpIndexParser(apParser), mStartedGetting(-1),
   mGetUpdated(0), mGetFailure(0), mNotModified(0)
{
    initializeSelf();
}

/*!
 *    デストラクタ
 *
 */
a2BCmodel::~a2BCmodel()
{
    // なにもしない
}

/*!
 *  クラスの初期化
 *
 */
void a2BCmodel::initializeSelf(void)
{
    // ニックネームをクリアする
    mNick.clear();
    
    // リストをクリアする
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
 *    bbstable.htmlファイルを選択する
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
 *    bbstable.htmlファイルをURLから取得する
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

    // 取得メイン...
    mpHttp->getFileFromUrl(arFileName, arUrl, false);
    return;
}

/*!
 *    bbstable.htmlファイルの取得をキャンセルする
 *
 */
void a2BCmodel::cancelGetBbsTable()
{
    return;
}



/*!
 *  存在する ディレクトリをチェックする
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
                // 更新するディレクトリだった、、、
                mNick.append(dirName);
            }
        }
    }
    return;
}


/*!
 *   a2Bのログディレクトリ部分を認識する
 * 
 * 
 */
bool a2BCmodel::prepareA2BLogDirectory(bbsTableParser &arBbsTable, QString &arBaseDirectory)
{
    QString message, fileName;

    // データを初期化する
    initializeSelf();

    // 板一覧データの読み込み準備
    fileName = mpDB->getBbsTableFileName();
    bool readyBbs = arBbsTable.prepare(fileName);
    if (readyBbs == false)
    {
        // 板一覧の解析失敗...終了する
        message = tr("bbs table read failure..."); 
        emit updateMessage(message);
        return (false);
    }
    int count = arBbsTable.numberOfBoards();
    message.setNum(count);
    message = tr("parsed board : ") + message;
    emit updateMessage(message);

    // 検索用ディレクトリ名を確定する
    arBaseDirectory = fileName;
    arBaseDirectory.replace(QRegExp("bbstable.html$"), "");

    // ログ格納ディレクトリの一覧を取得する
    QDir baseDir(arBaseDirectory);
    checkDirectoryList(baseDir);
    message = tr("Base directory : ") + arBaseDirectory; 
    emit updateMessage(message);

    /// お気に入りデータを読み込む...
    int favorCount = mFavoriteParser.parseIndexFile(arBaseDirectory + "favorite.idx");
    message.setNum(favorCount);
    message = tr(" favorite count : ") + message; 
    emit updateMessage(message);
    
    return (true);
}

/*
 *   a2b.idxのデータを管理する
 * 
 * 
 */
int a2BCmodel::parseA2Bindex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick, int aOffset)
{
    ////// a2b.idxを読んで、取得するファイルの一覧を作成する /////
    QString boardUrl    = arBbsTable.getUrl(arBoardNick);
    if (boardUrl.isEmpty() == true)
    {
        // 板URLの取得に失敗したとき...
        return (0);
    }
    QString targetIndex = arBaseDirectory + arBoardNick + "/a2b.idx";
    int count = mpIndexParser->parseIndexFile(targetIndex);

    ///// お気に入りデータに登録されているBBSのURLをbbstable.html と整合をとる...
    ///// (時間がかかるだろーなーー)
    mFavoriteParser.updateUrl(arBoardNick + "/", boardUrl);

    // 「ログファイルを再取得する」のチェック状態
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
            // ステータスが未取得、または、取得予約、エラーだった場合には全部取得する
            datItem->setPartGet(false);
        }
        else
        {
            // 差分取得を実施する
            datItem->setPartGet(true);
        }

        int dataNofMsg = mpIndexParser->getNofMessage(index + aOffset);
        if (dataNofMsg > 1000)
        {
            // レスが1000を超えていたスレは取得しない
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

    // a2b.idxを読み出した結果を表示する。
    QString message = "";
    message.setNum(count);
    message = "  " + targetIndex + " (" + message + ")";
//  emit updateMessage(message);

    return (count);
}

/*!
 *   「dat落ちしたスレも読めるようにする」にチェックされたときの処理
 * 
 */
void a2BCmodel::parseOldThreadFile(QString &arBaseDirectory, QString &arBoardNick)
{
    // "dat落ちしたスレも読めるようにする" で、dat探し機能を追加...
    QDir targetDir(arBaseDirectory + arBoardNick);
    QStringList filter;
    filter << "*.dat";
    QStringList nameList = targetDir.entryList(filter);
    QString temp;
    int ct = 0;
    for (QStringList::const_iterator ite = nameList.constBegin(); ite != nameList.constEnd(); ite++)
    {
        // indexファイルにない datファイルを発見した!!
        if (mpIndexParser->exists(*ite) < 0)
        {
            // ファイルチェッカに格納する
            if (mDatFileChecker.appendDataFile((arBaseDirectory + arBoardNick), (*ite)) == true)
            {
                temp = temp + (*ite) + " ";
                ct++;
            }
        }
    }

    if (ct != 0)
    {
        // a2b.idxに追加したファイルがあれば表示する
        QString message;
        message.setNum(ct);
        temp = arBoardNick + " (" + message + ") : " + temp;
        emit updateMessage(temp);
    }
    return;
}

/*!
 *  subject.txtも同時に更新するかどうかのチェック
 * 
 */
void a2BCmodel::parseSubjectTxtIndex(bbsTableParser &arBbsTable, QString &arBaseDirectory, QString &arBoardNick)
{
    // subject.txtも同時に更新するか？
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
 *  更新を実行するか確認する
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
    
    // ダイアログ、ボックスの確認
    if (mb.exec() != QMessageBox::Ok)
    {
        return (false);
    }
    return (true);
}

/*!
 *  a2Bのデータ更新メイン処理...
 *
 */
void a2BCmodel::update_a2B_datas(void)
{
    bbsTableParser bbsTable;
    QString        a2BDirectory;

    // a2Bのログディレクトリ関連の準備...
    if (prepareA2BLogDirectory(bbsTable, a2BDirectory) == false)
    {
        return;
    }

    // 板すべてについて実行する...
    int offset = 0;
    for (QVector<QString>::ConstIterator it = mNick.begin(); it != mNick.end(); it++)
    {
        QString boardNick = (*it);
        
        // "a2b.idx"についての解析処理
        offset = offset + parseA2Bindex(bbsTable, a2BDirectory, boardNick, offset);

        // "subject.txt"を更新するかどうか...
        parseSubjectTxtIndex(bbsTable, a2BDirectory, boardNick);

        // 「dat落ちしたスレを読めるようにする」にチェックがあるかどうか...
        // (「dat落ちしたスレも読めるようにする」にチェックされていないときにはなにもしない)
        if (mpDB->getEnableDownThread() != Qt::Unchecked)
        {
            parseOldThreadFile(a2BDirectory, boardNick);
        }
    }

    // ログ取得の開始を通知
    emit startedUpdate();

    // 取得を開始するかどうかを確認する
    if (confirmStartUpdate() == false)
    {
        // 何もせずに終了する
        mpIndexParser->clearIndexFile();

        // 終了を通知
        emit completedUpdate();
        return;
    }

    // 取得結果の初期化
    mGetUpdated  = 0;
    mGetFailure  = 0;
    mNotModified = 0;

    // bbstable.htmlにスレを取得するかどうか
    if (mpDB->getAddToBbsTable() == Qt::Checked)
    {
        // bbstable.htmlに取得済みスレを追加する場合...
        trimBbsTable(bbsTable);
    }

    // 初回のタイムアウト待ち
    unsigned long interval = mpDB->getReceiveInterval();
    mStartedGetting = 0;
    emit setTimer(0, interval);

    // プログレスバーの最大値設定
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
            // 登録データを格納する
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
 *    bbstable.htmlファイルの取得終了処理
 *
 */
void a2BCmodel::finishGetBbsTable(void)
{
    return;
}

/*!
 *   タイムアウトの受信 (subject.txt , レスの取得実施)
 * 
 */
void a2BCmodel::receiveTimeout(int id)
{
    QString message = "";
  
    if (id >= mGettingFileList.size())
    {
        // エラーが発生したときには、管理対象からはずすか？
        bool removeError = false;
        if (mpDB->getRemoveErrorMessage() == Qt::Checked)
        {
            removeError = true;
        }

        // 1000超えが発生したときには、管理対象からはずすか？
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

        // お気に入りデータがあった場合には、更新する
        if (mFavoriteParser.count() > 0)
        {
            mFavoriteParser.outputIndexFile(removeError, removeOver);
            message = "\n---\n" + tr("updated favorite file");
            emit updateMessage(message);
        }

        // ログ取得の終了。。
        message.setNum(mStartedGetting);

        mStartedGetting = -1;
        message = "\n------\n" + tr("complete to get log :") + message + "\n------\n";
        emit updateMessage(message);

        // 終了を通知する        
        emit completedUpdate();

        // ダイアログボックスでログ取得の終了を通知する
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

        // クラス内のデータをクリアする
        initializeSelf();
        return;
    }

    // 次に取得するログデータをとってくる
    a2BCgettingFile *itemData = mGettingFileList[id];

    // 区切り線を入れる
    message = "---";
    emit updateMessage(message);
 
    emit updateValue(id);
    
    // ログ取得情報をとってくる...
    QString fileName = itemData->getFileName();
    QString url      = itemData->getUrl();
    bool   partGet  = itemData->getPartGet();

    // 取得方法の確認
    if (partGet == true)
    {
        // 差分取得
        message = tr("update file: ");
    }
    else
    {
        // 全取得
        message = tr("get file: ");
    }

    // 表示
    message = message + itemData->getFileName();
    emit updateMessage(message);
    
    // ログ取得を呼び出す
    mpHttp->getFileFromUrl(fileName, url, partGet);
 
    return;
}

/*!
 *   通信の終了
 * 
 */
void a2BCmodel::completedCommunication(int error)
{
    // 新着確認を実施しているかチェックする
    if (mStartedGetting < 0)
    {
        // 新着確認未実施...なので終了する
        return;
    }
    
    int status = A2BCSTATUS_UNKNOWN;
    int index = mGettingFileIndex[mStartedGetting];
    if (index >= 0)
    {
        // 通信結果を反映させる
        if (error > 0)
        {
            // 通信成功
            status = A2BCSTATUS_UPDATE;
            mGetUpdated++;
        }
        else if (error == 0)
        {
            // 更新なし、、状態は変更しない
            status = A2BCSTATUS_UNKNOWN;
            mNotModified++;
        }
        else
        {
            // 通信失敗、エラー状態に更新する
            status = A2BCSTATUS_GETERROR;
            mGetFailure++;
        }

        quint64 bbsTime = mpIndexParser->getLastUpdate(index);  // 更新前の時刻を取得...
        mpIndexParser->setFileStatus(index, status);
        
        int dataNofMsg = mpIndexParser->getNofMessage(index);
        QString dataIndexFile = mpIndexParser->getFileName(index);
        QString boardNick = mpIndexParser->getNickName(index);

        /// お気に入り登録スレかどうかチェックし、データを同期させる
        int favorIndex = mFavoriteParser.exists(boardNick, dataIndexFile);
        if (favorIndex >= 0)
        {
            // お気に入りなので、マークをつける
            mFavoriteParser.markIt(favorIndex);

            // ファイル状態を更新する
            mFavoriteParser.setFileStatus(favorIndex, status);

            // お気に入りチェック
            QString message = tr("---");
            emit updateMessage(message);

            // お気に入りに登録されていたスレだった...
            quint64 favorTime = mFavoriteParser.getLastUpdate(favorIndex);
            int nofMsg       = mFavoriteParser.getNofMessage(favorIndex);
            if (favorTime > bbsTime)
            {
                // お気に入り -> 通常板にデータ反映
                int curMsg = mFavoriteParser.getCurrentMessage(favorIndex);
                mpIndexParser->setNofMessage(index, nofMsg);
                mpIndexParser->setCurrentMessage(index, curMsg);
                mpIndexParser->setFileStatus(index, status);
        
                message = tr("modify favorite -> board");
                emit updateMessage(message);
            }
            else
            {
                // 通常板 -> お気に入りにデータ反映
                int curMsg = mpIndexParser->getCurrentMessage(index);
                mFavoriteParser.setNofMessage(favorIndex, dataNofMsg);
                mFavoriteParser.setCurrentMessage(favorIndex, curMsg);

                message = tr("modify board -> favorite");
                emit updateMessage(message);
            }
        }
    }

    // 次のログ取得のためにイベントを発行する
    QEvent *event = new QEvent(QEvent::User);
    QCoreApplication::postEvent(this, event);
    return;
}

/*!
 *    次の通信開始...
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
 *  イベントの受信
 * 
 */
void a2BCmodel::customEvent(QEvent *event)
{
    // null なら何もしない
    if (event == 0)
    {
        return;
    }
    
    // ユーザ定義イベントがきたら、次の通信を行う
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
