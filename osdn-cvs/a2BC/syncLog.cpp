/**
 *  ログ同期クラス(2ちゃんねるブラウザ間でdatファイルをコピーする)
 *   
 * 
 */
#include <QSet>
#include <QFile>
#include <QMessageBox>
#include "syncLog.h"
#include "screenData.h"
#include "a2BcConstants.h"

/**
 *  コンストラクタ
 * 
 */
syncLog::syncLog(QWidget *parent)
 : QWidget(parent), mpBrowserParser(0), mpA2BbrowserParser(0), mIsReady(false)
{
    // ここでは何もしない..
}

/*!
 *   デストラクタ
 * 
 */
syncLog::~syncLog()
{
    mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
    mOtherBrowserFactory.deleteOtherBrowserFactory(mpBrowserParser);
}


/*!
 *  実行の準備
 * 
 */
bool syncLog::prepare(screenData &arDB)
{
    QString message;

    // 板一覧データの読み込み準備
    QString fileName = arDB.getBbsTableFileName();
    bool readyBbs = mBbsTableParser.prepare(fileName);
    if (readyBbs == false)
    {
        // 板一覧の解析失敗...終了する
        return (false);
    }
    message = tr("read :") + " " + fileName;
    emit updateMessage(message);

    // a2B用のログ解析クラスを取得、準備を実行する
    mpA2BbrowserParser = mOtherBrowserFactory.createOtherBrowserFactory(OTHERBROWSER_A2B);
    if (mpA2BbrowserParser == 0)
    {
        return (false);
    }
    QString baseDirectory = fileName;
    baseDirectory.replace(QRegExp("bbstable.html$"), "");
    mpA2BbrowserParser->setPath(baseDirectory);
    mpA2BbrowserParser->setBbsTable(&mBbsTableParser, fileName);
    if (mpA2BbrowserParser->prepare() == false)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        return (false);
    }
    
    // 解析結果(datファイル数)を表示する
    message.setNum(mpA2BbrowserParser->datFileCount());
    message = tr("parsed ") + mpA2BbrowserParser->getBrowserName() + " : " + message;
    emit updateMessage(message);

    // 同期用のログ解析クラスを取得、準備を実行
    mpBrowserParser = mOtherBrowserFactory.createOtherBrowserFactory(arDB.getOtherBrowserType());
    if (mpBrowserParser == 0)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        return (false);
    }
    QString logPath = arDB.getOtherBrowserDirectory();
    mpBrowserParser->setPath(logPath);
    mpBrowserParser->setBbsTable(&mBbsTableParser, fileName);
    if (mpBrowserParser->prepare() == false)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpBrowserParser);
        return (false);
    }

    // 解析結果(datファイル数)を表示する
    message.setNum(mpBrowserParser->datFileCount());
    message = tr("parsed ") + mpBrowserParser->getBrowserName() + " : " + message;
    emit updateMessage(message);

    mIsReady = true;
    return (mIsReady);
}

/*!
 *   同期元(a2B)のブラウザ名を取得する
 * 
 */
QString syncLog::getSourceBrowserName()
{
    if (mIsReady == false)
    {
        return ("(not ready)");
    }
    QString message;
    message = mpA2BbrowserParser->getBrowserName();
    return (message);
}

/*!
 *   同期相手のブラウザ名を取得する
 * 
 */
QString syncLog::getDestinationBrowserName()
{
    if (mIsReady == false)
    {
        return ("(not ready)");
    }
    QString message;
    message = mpBrowserParser->getBrowserName();
    return (message);
}

/*!
 *   a2Bで管理されていないdatファイルを他ブラウザに移動させる
 * 
 */
void syncLog::moveToUnmanagedLogFile(int aSyncType)
{
    bool force = false;
    if (aSyncType == SYNC_FROM_a2B)
    {
        // a2Bから上書きする場合...
        force = true;
    }

    // a2Bで管理されていないdatファイルの数を求める
    int count = mpA2BbrowserParser->checkUnmanagedDatFile();
    for (int loop = 0; loop < count; loop++)
    {
        QString nickName = mpA2BbrowserParser->getUnmanagedDatNickName(loop);
        QString fileName = mpA2BbrowserParser->getUnmanagedDatFileName(loop);
        QString srcName  = mpA2BbrowserParser->getFullPathDatFileName(nickName, fileName);
        QString dstName  = mpBrowserParser->getReceiveDatFileName(nickName, fileName);
        QString message = "\n  " + srcName + " ==> " + dstName + "\n";
        emit updateMessage(message);
        
        // コピー先ファイルが存在していたとき...
        if (QFile::exists(dstName) == true)
        {
            // overwriteするかどうかの確認を行う...
            int ans = QMessageBox::No;
            if (force != true)
            {
                message = nickName + "/" + dstName + " " + tr("is already exists, overwrite it?");
                ans = QMessageBox::question(0, tr("confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::YesAll);
                if (ans == QMessageBox::YesAll)
                {
                    force = true;
                }
            }
            if ((force == true)||(ans == QMessageBox::Yes))
            {
                // 上書き確認でYesを選んだ場合、あるいは、YesAllを選んだ場合には、ファイルを消してコピーする
                QFile::remove(dstName);
            }
        }
        // ファイルを移動させる
        mpBrowserParser->receiveDatFile(nickName, fileName, dstName, srcName, mpA2BbrowserParser->isSameLogFormat(), 1, 0);
        message = tr("Move log") + " " + srcName + " ==> " + dstName + "  (" + nickName + ")";
        emit updateMessage(message);
    }
    return;
}

/*!
 *   同期の実行
 * 
 */
bool syncLog::doSync(int aSyncType, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt)
{
    bool ret = false;
    
    // a2Bのエラーログを他ブラウザへ移動させるかどうか...
    if (aBackupErrorLog == Qt::Checked)
    {
        // a2Bで管理されていないdatファイルを他ブラウザに移動させる場合...
        moveToUnmanagedLogFile(aSyncType);        
    }

    QString message;
    switch (aSyncType)
    {
      case SYNC_TO_a2B:
        // 他ブラウザ → a2B
        message = tr("Sync ") + getDestinationBrowserName() + " ==> " + getSourceBrowserName();
        emit updateMessage(message);
        ret = logCopyMain(mpA2BbrowserParser, mpBrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;

      case SYNC_FROM_a2B:
        // a2B → 他ブラウザ
        message = tr("Sync ") + getSourceBrowserName() + " ==> " + getDestinationBrowserName();
        emit updateMessage(message);
        ret = logCopyMain(mpBrowserParser, mpA2BbrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;

      case SYNC_ALL:
      default:
        // ログを同期させる場合...
        message = tr("Sync ") + getSourceBrowserName() + " <==> " + getDestinationBrowserName();
        emit updateMessage(message);
        ret = synchronizeMain(mpBrowserParser, mpA2BbrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;
    }

    // あとしまつ
    mpA2BbrowserParser->finalize();
    mpBrowserParser->finalize();

    message = tr("synchronize end...");
    emit completedSynchronize(message);
    return (ret);
}

/*!
 *   ログ同期の実行...
 * 
 */
bool syncLog::synchronizeMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt)
{
    bool ret = false;

    return (ret);
}

/*!
 *   ログコピーの実行...
 * 
 */
bool syncLog::logCopyMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aRemoveErrorLog, Qt::CheckState aCopySubjectTxt)
{
    QSet<QString>  nickSet;
    bool force = false;
    bool ret = false;

    // 変数をクリア
    nickSet.clear();

    // プログレスバーの最大値設定
    emit setMaximum(apSrc->datFileCount() - 2);

    // apSrc → apDstのログ上書きコピー...
    int count = apSrc->datFileCount();
    for (int loop = 0; loop < count; loop++)
    {
        QString srcNick    = apSrc->getBoardNick(loop);
        QString srcFile    = apSrc->getDatFileName(loop);
        int    readPoint  = apSrc->getReadPoint(loop);
        qint64  lastUpdate = apSrc->getLastUpdate (loop);
        QString srcName    = apSrc->getFullPathDatFileName(srcNick, srcFile);
        QString dstName    = apDst->getReceiveDatFileName(srcNick, srcFile);

        if ((aCopySubjectTxt == Qt::Checked)&&(nickSet.contains(srcNick) != true))
        {
            // ニックネームが記録されていなかった...
            nickSet.insert(srcNick);
        }

        // コピー先ファイル名を生成する...
        int ans = QMessageBox::Yes;
        if (QFile::exists(dstName) == true)
        {
            // overwriteするかどうかの確認を行う...
            if (force != true)
            {
                QFile srcFileInfo(srcName);
                QFile dstFileInfo(dstName);
                qint64 srcSize, dstSize;
                QString srcNum, dstNum;
                srcSize = srcFileInfo.size();
                dstSize = dstFileInfo.size();
                srcNum.setNum(srcSize);
                dstNum.setNum(dstSize);
                QString message = dstName + " " + tr("is already exists, overwrite it?") +"\n\n\t";
                message = message +  tr("src File") + " : " + srcNum + " bytes\n\t" +  tr("dst File") + " : " + dstNum + " bytes";
                if (srcSize == dstSize)
                {
                    message = message + "\n\n" + tr("file size is same.");
                }
                if (srcSize < dstSize)
                {
                    message = message + "\n\n" + tr("the destination file is bigger.");
                }
                ans = QMessageBox::question(0, tr("confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::YesAll);
                if (ans == QMessageBox::YesAll)
                {
                    force = true;
                    ans = QMessageBox::Yes;
                }
            }
            if ((force == true)||(ans == QMessageBox::Yes))
            {
                // 上書き確認でYesを選んだ場合、あるいは、YesAllを選んだ場合には、ファイルを消してコピーする
                QFile::remove(dstName);
            }
        }
        if (ans == QMessageBox::Yes)
        {
            // ファイルをコピーする
            apDst->copyDatFile(srcNick, srcFile, dstName, srcName, apSrc->isSameLogFormat(), readPoint, lastUpdate);
            QString message = "\n  " + srcName + " --> " + dstName + "\t  (" + srcNick + ")";
            emit updateMessage(message);
        }
        
        // プログレスバーの更新...
        emit updateValue(loop);
    }

    // subject.txtもコピーする指定があった場合。。。
    if (aCopySubjectTxt == Qt::Checked)
    {
        QSet<QString>::const_iterator item = nickSet.begin();
        while (item != nickSet.end())
        {
            QString nick     = *item;
            QString srcFile  = apSrc->getSubjectTxtFilePath(nick);
            QString dstFile  = apDst->getSubjectTxtFilePath(nick);
            if (QFile::exists(dstFile) == true)
            {
                // ファイルが存在した場合には削除してからコピーする...
                QFile::remove(dstFile);
            }
            QFile::copy(srcFile, dstFile);
            QString message = "\n  " + srcFile + " -> " + dstFile;
            emit updateMessage(message);

            item++;
        }
    }
    return (ret);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
