#ifndef A2BC_FAVORITE_EDITOR_H
#define A2BC_FAVORITE_EDITOR_H

#include <QWidget>
#include <QStandardItemModel>
#include <QItemDelegate>
#include "a2BCfavoriteParser.h"

/*!
 *  a2BCfavoriteEditor
 * 
 */
class screenData;
class QTableView;
class QHeaderView;
class a2BCfavoriteItemModel;
class a2BCfavoriteItemDelegate;
class a2BCfavoriteEditor : public QObject
{
    Q_OBJECT

public:
    a2BCfavoriteEditor(QObject *parent = 0);
    ~a2BCfavoriteEditor();

public:
    void prepare(screenData *apDB, QTableView *apView);

signals:
    void updateMessage(const QString &);

public slots:
    void extractFavoriteData();
    void storeFavoriteData();
    void removeFavoriteData();
    void swapFavoriteData();

public:

private:

private:
    a2BCfavoriteParser        mFavoriteParser;
    QTableView               *mpItemView;
    QHeaderView              *mpHeaderView;
    a2BCfavoriteItemModel    *mpItemModel;
    a2BCfavoriteItemDelegate *mpItemDelegate;
    screenData               *mpDB;
};
#endif // #ifndef A2BC_FAVORITE_EDITOR_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
