/*!
 *   データ保持部..
 *
 */
#include "screenData.h"

/*!
 *  コンストラクタ
 *
 */
screenData::screenData()
{
    bbsTableUrl              = "http://menu.2ch.net/bbstable.html";
    bbsTableFileName         = "";
    otherBrowserDirectory    = "";
    updateSubjectTxt         = Qt::Checked;
    enableDownThread         = Qt::Unchecked;
    addThreadToIndex         = Qt::Unchecked;
    addToBbsTable            = Qt::Unchecked;
    disableGetPart           = Qt::Unchecked;
    removeErrorThreadMessage = Qt::Unchecked;
    removeOverThreadMessage  = Qt::Unchecked;
    backupErrorLog           = Qt::Unchecked;
    copySubjectTxt           = Qt::Unchecked;
    getInterval              = 500;
    otherBrowserType         = 0;
    synchronizeType          = 0;

}

/*!
 *   bbstable.htmlの取得先URLを応答する
 *
 */
QString screenData::getBbsTableUrl(void)
{
    return (bbsTableUrl);
}

/*!
 *   bbstable.htmlの取得先URLを記憶する
 *
 */
void screenData::setBbsTableUrl(const QString &arUrl)
{
    bbsTableUrl = arUrl;
    return;
}

/*!
 *   bbstable.htmlのファイル名を応答する
 *
 */
QString screenData::getBbsTableFileName(void)
{
    return (bbsTableFileName);
}

/*!
 *  他2ちゃんブラウザの管理ディレクトリを応答する
 * 
 */
QString screenData::getOtherBrowserDirectory(void)
{
    return (otherBrowserDirectory);
}

/*!
 *  他2ちゃんブラウザの管理ディレクトリを記憶する
 * 
 */
void screenData::setOtherBrowserDirectory(const QString &arDirectory)
{
    otherBrowserDirectory = arDirectory;
    return;
}


/*!
 *   bbstable.htmlのファイル名を記憶する
 *
 */
void screenData::setBbsTableFileName(const QString &arFileName)
{
    bbsTableFileName = arFileName;
    return;
}

/*!
 *   subject.txtも最新に更新するか？の状態を応答する
 *
 */
Qt::CheckState screenData::getUpdateSubjectTxt(void)
{
    return (updateSubjectTxt);
}

/*!
 *   subject.txtも最新に更新するか？の状態を更新する
 *
 */
void screenData::setUpdateSubjectTxt(Qt::CheckState yesNo)
{
    updateSubjectTxt = yesNo;
}

/*!
 *   新着確認をお気に入り一覧だけにするか？の状態を応答する
 *
 */
Qt::CheckState screenData::getEnableDownThread(void)
{
    return (enableDownThread);
}

/*!
 *   新着確認をお気に入り一覧だけにするか？の状態を更新する
 *
 */
void screenData::setEnableDownThread(Qt::CheckState yesNo)
{
    enableDownThread = yesNo;
}

/*!
 *   迷子スレを管理対象とするか？の状態を応答する
 *
 */
Qt::CheckState screenData::getAddThreadToIndex(void)
{
    return (addThreadToIndex);
}

/*!
 *   管理対象スレがsubject.txtになかった場合、追加するか？の状態を更新する
 *
 */
void screenData::setAddThreadToIndex(Qt::CheckState yesNo)
{
    addThreadToIndex = yesNo;
}

/*!
 *   管取得スレがある板を「お気に入り板」としてbbstable.htmlに追加するか？の状態を応答する
 *
 */
Qt::CheckState screenData::getAddToBbsTable(void)
{
    return (addToBbsTable);
}

/*!
 *  取得スレがある板を「お気に入り板」としてbbstable.htmlに追加するか？の状態を更新する
 *
 */
void screenData::setAddToBbsTable(Qt::CheckState yesNo)
{
    addToBbsTable = yesNo;
}

/*!
 *   スレをすべて再取得するか？の状態を応答する
 *
 */
Qt::CheckState screenData::getDisableGetPart(void)
{
    return (disableGetPart);
}

/*!
 *   スレをすべて再取得するか？の状態を更新する
 *
 */
void screenData::setDisableGetPart(Qt::CheckState yesNo)
{
    disableGetPart = yesNo;
}

/*!
 *   エラースレを管理対象からはずすか？の状態を応答する
 *
 */
Qt::CheckState screenData::getRemoveErrorMessage(void)
{
    return (removeErrorThreadMessage);
}

/*!
 *   エラースレを管理対象からはずすか？の状態を更新する
 *
 */
void screenData::setRemoveErrorMessage(Qt::CheckState yesNo)
{
    removeErrorThreadMessage = yesNo;
}

/*!
 *   1000超えスレを管理対象からはずすか？の状態を応答する
 *
 */
Qt::CheckState screenData::getRemoveOverMessage(void)
{
    return (removeOverThreadMessage);
}

/*!
 *   1000超えスレを管理対象からはずすか？の状態を更新する
 *
 */
void screenData::setRemoveOverMessage(Qt::CheckState yesNo)
{
    removeOverThreadMessage = yesNo;
}


/*!
 *   新着確認の取得間隔を応答する（単位：ms）
 *
 */
int screenData::getReceiveInterval(void)
{
    return (getInterval);
}


/*!
 *   新着確認の取得間隔を更新する（単位：ms）
 *
 */
void screenData::setReceiveInterval(int interval)
{
    getInterval = (interval < 100) ? 100: interval;
    return;
}

/*!
 *  a2Bでエラーが出たログを移動させる設定
 * 
 */
Qt::CheckState screenData::getBackupErrorLog(void)
{
    return (backupErrorLog);
}

/*!
 *   a2Bでエラーが出たログを移動させる設定
 * 
 */
void screenData::setBackupErrorLog(Qt::CheckState yesNo)
{
    backupErrorLog = yesNo;
}

/*!
 *  a2Bで1000を超えたスレを移動させる設定
 * 
 */
Qt::CheckState screenData::getBackupOverLog(void)
{
    return (backupOverLog);
}

/*!
 *   a2Bで1000を超えたスレを移動させる設定
 * 
 */
void screenData::setBackupOverLog(Qt::CheckState yesNo)
{
    backupOverLog = yesNo;
}

/*!
 *   subject.txtも同時にコピーするか？の状態を応答する
 *
 */
Qt::CheckState screenData::getCopySubjectTxt(void)
{
    return (copySubjectTxt);
}

/*!
 *   subject.txtも同時にコピーするか？の状態を更新する
 *
 */
void screenData::setCopySubjectTxt(Qt::CheckState yesNo)
{
    copySubjectTxt = yesNo;
}

/*!
 *  他２ちゃんねるブラウザのタイプを取得する
 * 
 */
int screenData::getOtherBrowserType(void)
{
    return (otherBrowserType);
}

/*!
 *  他２ちゃんねるブラウザのタイプを設定する
 * 
 */
void screenData::setOtherBrowserType(int browserType)
{
    otherBrowserType = browserType;
    return;  
}

/*!
 *  他２ちゃんねるブラウザのタイプを取得する
 * 
 */
int screenData::getSynchronizeType(void)
{
    return (synchronizeType);
}

/*!
 *  他２ちゃんねるブラウザのタイプを設定する
 * 
 */
void screenData::setSynchronizeType(int browserType)
{
    synchronizeType = browserType;
    return;  
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
