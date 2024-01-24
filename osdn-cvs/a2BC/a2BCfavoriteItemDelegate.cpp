/*!
 *   a2BCfavoriteItemDelegate
 * 
 *    [説明]
 *      お気に入り編集用データ加工クラス
 * 
 */
#include <QSpinBox>
#include <QModelIndex>
#include "a2BCfavoriteItemDelegate.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCfavoriteItemDelegate::a2BCfavoriteItemDelegate(QObject *parent)
 : QItemDelegate(parent)
{
    //
}

/*!
 *  デストラクタ
 * 
 */
a2BCfavoriteItemDelegate::~a2BCfavoriteItemDelegate()
{
    //
}

/*!
 *
 *
 */
QWidget *a2BCfavoriteItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int column = index.column();
    if (column == 1)
    {
        // スレ取得数、編集できない
        return (0);
       }
       else if (column == 5)
       {
        // お気に入りレベル、0～9の範囲
        QSpinBox *editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(9);
        editor->installEventFilter(const_cast<a2BCfavoriteItemDelegate *>(this));
        return (editor);
      }
    return (QItemDelegate::createEditor(parent, option, index));
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
