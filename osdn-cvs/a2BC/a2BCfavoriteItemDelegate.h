#ifndef A2BC_FAVORITE_DELEGATE_H
#define A2BC_FAVORITE_DELEGATE_H

#include <QWidget>
#include <QItemDelegate>

/*!
 *  a2BCfavoriteItemDelegate
 * 
 */
class QModelIndex;
class a2BCfavoriteItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    a2BCfavoriteItemDelegate(QObject *parent = 0);
    virtual ~a2BCfavoriteItemDelegate();

public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

    
};
#endif // #ifndef A2BC_FAVORITE_DELEGATE_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
