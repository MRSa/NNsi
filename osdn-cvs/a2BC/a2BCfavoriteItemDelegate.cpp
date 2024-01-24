/*!
 *   a2BCfavoriteItemDelegate
 * 
 *    [����]
 *      ���C�ɓ���ҏW�p�f�[�^���H�N���X
 * 
 */
#include <QSpinBox>
#include <QModelIndex>
#include "a2BCfavoriteItemDelegate.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCfavoriteItemDelegate::a2BCfavoriteItemDelegate(QObject *parent)
 : QItemDelegate(parent)
{
    //
}

/*!
 *  �f�X�g���N�^
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
        // �X���擾���A�ҏW�ł��Ȃ�
        return (0);
       }
       else if (column == 5)
       {
        // ���C�ɓ��背�x���A0�`9�͈̔�
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
