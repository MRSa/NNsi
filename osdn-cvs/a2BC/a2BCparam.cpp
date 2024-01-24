/*!
 *   パラメータ読み出し・記憶部
 *
 */
#include <Qt>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QXmlSimpleReader>
#include "a2BCparam.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCparam::a2BCparam(QWidget *parent)
  : QWidget(parent), mLoaded(false), mItemParam(false), mPath(0), mpDB(0)
{

}

/*!
 *   デストラクタ
 * 
 */
a2BCparam::~a2BCparam()
{

}

/*!
 *  ファイルから読み出す
 * 
 */
bool a2BCparam::loadFromFile(QString &execPath, screenData *arDB)
{
    bool ret = false;
    mpDB  = arDB;
    mPath = execPath;

    QFile file(mPath + "/a2BC-conf.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        // ファイルオープン失敗...
        return (false);
    }
    QString     itemName  = "";
    QString     itemType  = "";
    QString     itemValue = "";
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<NNsi:item", Qt::CaseInsensitive) == true)
        {
            mItemParam = true;
            itemName = line.section("\"", 1, 1);
            itemType = line.section("\"", 3, 3);
        }
        if ((mItemParam == true)&&(line.contains("<NNsi:val>", Qt::CaseInsensitive) == true))
        {
            itemValue  = line.section("\"", 1, 1);
            bool result = setParameter(itemName, itemType, itemValue);
            if (result == true)
            {
                ret = true;
            }
            itemName   = "";
            itemType   = "";
            itemValue  = "";
            mItemParam = false;
        }
    }
    file.close();
    return (ret);
}

/*!
 *   ファイルへ記録する
 * 
 */
bool a2BCparam::saveToFile(void)
{
    if (mpDB == 0)
    {
        return (false);
    }

    QFile file(mPath + "/a2BC-conf.xml");
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        // ファイルオープン失敗
        QMessageBox::warning(this, tr("a2BC preference"),
                             tr("Cannot write file %1:\n%2.")
                             .arg("a2BC-conf.xml")
                             .arg(file.errorString()));
        return (false);
    }
    QTextStream out;
    out.setDevice(&file);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE NNsi:NNsiXML SYSTEM \"http://nnsi.sourceforge.jp/NNsiXML/NNsiXML100.dtd\">\n"
        << "<NNsi:NNsiXML xmlns:NNsi=\"http:nnsi.sourceforge.jp/NNsiXML\">\n"
        << "  <NNsi:NNsiSet ver=\"100\">\n";
 
    //////////  bbstable.html 取得先URL  //////////
    out << "    <NNsi:item name=\"bbsTableUrl\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getBbsTableUrl() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  bbstable.html ローカル格納ファイル  //////////
    out << "    <NNsi:item name=\"bbsTableFileName\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getBbsTableFileName() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  取得間隔 (単位：ms)  //////////
    out << "    <NNsi:item name=\"getReceiveInterval\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getReceiveInterval() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  スレ一覧を同時に更新する (true: 更新する, false: 更新しない)  //////////
    out << "    <NNsi:item name=\"updateSubjectTxt\" type=\"bool\">\n";
    if (mpDB->getUpdateSubjectTxt() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  dat落ちしたスレも読めるようにする (true:読めるようにする, false:しない) //////////
    out << "    <NNsi:item name=\"enableDownThread\" type=\"bool\">\n";
    if (mpDB->getEnableDownThread() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  (未使用、迷子スレを管理対象にする)  //////////
    out << "    <NNsi:item name=\"addThreadToIndex\" type=\"bool\">\n";
    if (mpDB->getAddThreadToIndex() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  エラーが発生したスレを他ブラウザに移動させる  //////////
    out << "    <NNsi:item name=\"backupErrorLog\" type=\"bool\">\n";
    if (mpDB->getBackupErrorLog() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  1000超えが発生したスレを他ブラウザに移動させる  //////////
    out << "    <NNsi:item name=\"backupOverLog\" type=\"bool\">\n";
    if (mpDB->getBackupOverLog() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  bbstable.htmlにa2Bカテゴリを追加する (true:追加する, false:追加しない)  //////////
    out << "    <NNsi:item name=\"addToBbsTable\" type=\"bool\">\n";
    if (mpDB->getAddToBbsTable() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  スレを全て再取得する (true:再取得する, false:再取得しない)  //////////
    out << "    <NNsi:item name=\"disableGetPart\" type=\"bool\">\n";
    if (mpDB->getDisableGetPart() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  エラースレを管理対象からはずす (true:外す, false:外さない)  //////////
    out << "    <NNsi:item name=\"removeErrorThreadMessage\" type=\"bool\">\n";
    if (mpDB->getRemoveErrorMessage() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  1000超えスレを管理対象からはずす (true:外す, false:外さない)  //////////
    out << "    <NNsi:item name=\"removeOverThreadMessage\" type=\"bool\">\n";
    if (mpDB->getRemoveOverMessage() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  スレ一覧を同時にコピーする (true: コピーする, false: コピーしない)  //////////
    out << "    <NNsi:item name=\"copySubjectTxt\" type=\"bool\">\n";
    if (mpDB->getCopySubjectTxt() == Qt::Checked)
    {
        out << "      <NNsi:val> \"true\" </NNsi:val>\n";
    }
    else
    {
        out << "      <NNsi:val> \"false\" </NNsi:val>\n";
    }
    out << "    </NNsi:item>\n";

    //////////  他２ちゃんブラウザの管理ディレクトリ  //////////
    out << "    <NNsi:item name=\"otherBrowserDirectory\" type=\"QString\">\n"
        << "      <NNsi:val> \"" << mpDB->getOtherBrowserDirectory() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  他２ちゃんブラウザのタイプ  //////////
    out << "    <NNsi:item name=\"otherBrowserType\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getOtherBrowserType() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    //////////  他２ちゃんブラウザとの同期方式  //////////
    out << "    <NNsi:item name=\"synchronizeType\" type=\"int\">\n"
        << "      <NNsi:val> \"" << mpDB->getSynchronizeType() << "\" </NNsi:val>\n"
        << "    </NNsi:item>\n";

    out << "  </NNsi:NNsiSet>\n"
        << "</NNsi:NNsiXML>\n";
    file.flush();
    file.close();
    return (true);
}

/*!
 *   パラメータを設定する
 * 
 */
bool a2BCparam::setParameter(QString name, QString type, QString value)
{
    bool ret = false;
    
    //////////  bbstable.html 取得先URL  //////////
    if (name.contains("bbsTableUrl", Qt::CaseInsensitive) == true)
    {
        mpDB->setBbsTableUrl(value);
    }

    //////////  bbstable.html ローカル格納ファイル  //////////
    if (name.contains("bbsTableFileName", Qt::CaseInsensitive) == true)
    {
        mpDB->setBbsTableFileName(value);
        if (value.contains("bbstable.html") == true)
        {
            ret = true;
        }
    }

    //////////  取得間隔 (単位：ms)  //////////
    if (name.contains("getReceiveInterval", Qt::CaseInsensitive) == true)
    {
        mpDB->setReceiveInterval(value.toInt());
    }

    //////////  スレ一覧を同時に更新する (true: 更新する, false: 更新しない)  //////////
    if (name.contains("updateSubjectTxt", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setUpdateSubjectTxt(yesNo);
    }

    //////////  dat落ちしたスレも読めるようにする (true:読めるようにする, false:しない) //////////
    if (name.contains("enableDownThread", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setEnableDownThread(yesNo);
    }

    //////////  (未使用、迷子スレを管理対象にする)  //////////
    if (name.contains("addThreadToIndex", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setAddThreadToIndex(yesNo);
    }

    //////////  エラーが発生したスレを他ブラウザに移動させる (true:移動する, false:移動しない)  //////////
    if (name.contains("backupErrorLog", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setBackupErrorLog(yesNo);
    }

    //////////  1000超えが発生したスレを他ブラウザに移動させる (true:移動する, false:移動しない)  //////////
    if (name.contains("backupOverLog", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setBackupOverLog(yesNo);
    }

    //////////  bbstable.htmlにa2Bカテゴリを追加する (true:追加する, false:追加しない)  //////////
    if (name.contains("addToBbsTable", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setAddToBbsTable(yesNo);
    }

    //////////  スレを全て再取得する (true:再取得する, false:再取得しない)  //////////
    if (name.contains("disableGetPart", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setDisableGetPart(yesNo);
    }

    //////////  エラースレを管理対象からはずす (true:外す, false:外さない)  //////////
    if (name.contains("removeErrorThreadMessage", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setRemoveErrorMessage(yesNo);
    }

    //////////  1000超えスレを管理対象からはずす (true:外す, false:外さない)  //////////
    if (name.contains("removeOverThreadMessage", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setRemoveOverMessage(yesNo);
    }

    //////////  スレ一覧を同時に更新する (true: 更新する, false: 更新しない)  //////////
    if (name.contains("copySubjectTxt", Qt::CaseInsensitive) == true)
    {
        Qt::CheckState yesNo = Qt::Unchecked;
        if (value.contains("true", Qt::CaseInsensitive) == true)
        {
            yesNo = Qt::Checked;
        }
        mpDB->setCopySubjectTxt(yesNo);
    }

    //////////  他２ちゃんブラウザの管理ディレクトリ  //////////
    if (name.contains("otherBrowserDirectory", Qt::CaseInsensitive) == true)
    {
        mpDB->setOtherBrowserDirectory(value);
    }

    //////////  他２ちゃんブラウザのタイプ  //////////
    if (name.contains("otherBrowserType", Qt::CaseInsensitive) == true)
    {
        mpDB->setOtherBrowserType(value.toInt());
    }

    //////////  他２ちゃんブラウザのタイプ  //////////
    if (name.contains("synchronizeType", Qt::CaseInsensitive) == true)
    {
        mpDB->setSynchronizeType(value.toInt());
    }

    return (ret);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
