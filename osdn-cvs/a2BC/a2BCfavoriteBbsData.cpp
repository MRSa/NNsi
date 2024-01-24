/**
 *   a2BCfavoriteBbsData
 * 
 *    [説明]
 *      お気に入り板データ管理クラス
 * 
 */
#include <QString>
#include <QStandardItem>
#include "a2BCfavoriteBbsData.h"

/**
 *  コンストラクタ
 * 
 */
a2BCfavoriteBbsData::a2BCfavoriteBbsData(QObject *parent) : QObject(parent)
{
    //
    mpName    = 0;
    mpUrl     = 0;
    mpSortKey = 0; 
}

/**
 *  デストラクタ
 * 
 */
a2BCfavoriteBbsData::~a2BCfavoriteBbsData()
{
/**
    if (mpName != 0)
    {
        delete mpName;
    }
    if (mpUrl != 0)
    {
        delete mpUrl;
    }
    if (mpSortKey != 0)
    {
        delete mpSortKey;
    }
**/
}

/**
 *   データの設定
 * 
 */
void a2BCfavoriteBbsData::setData(const QString &arName, const QString &arUrl, const QString &arKey)
{
    mpName    = new QStandardItem(arName);
    mpUrl     = new QStandardItem(arUrl);
    mpSortKey = new QStandardItem(arKey);    
}

/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
