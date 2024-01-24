#ifndef A2BC_FAVORITE_MODEL_H
#define A2BC_FAVORITE_MODEL_H

#include <QWidget>
#include <QStandardItemModel>

/*!
 *  a2BCfavoriteItemModel
 * 
 */
class a2BCfavoriteItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    a2BCfavoriteItemModel(QObject *parent = 0);
    a2BCfavoriteItemModel(int rows, int columns, QObject *parent = 0);
    virtual ~a2BCfavoriteItemModel();

public:
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual void     setHeaderLabel(const QStringList &headerList);
    virtual void     mysort(int column, Qt::SortOrder order);
    virtual void     myswapData(int data1, int data2);

private:
    int  compareData(int column, int data1, int data2);

private:
    QStringList headerLabel;
    
};
#endif // #ifndef A2BC_FAVORITE_MODEL_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
