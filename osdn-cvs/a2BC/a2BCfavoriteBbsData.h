#ifndef A2BC_FAVORITE_BBS_DATA_H
#define A2BC_FAVORITE_BBS_DATA_H

#include <QWidget>

/**
 *  a2BCfavoriteBbsData
 * 
 */
class QStandardItem;
class a2BCfavoriteBbsData : public QObject
{
    Q_OBJECT

public:
    a2BCfavoriteBbsData(QObject *parent = 0);
    ~a2BCfavoriteBbsData();

public:
    void setData(const QString &arName, const QString &arUrl, const QString &arKey);

public:
    QStandardItem *getName() { return (mpName); };
    QStandardItem *getUrl() { return (mpUrl); };
    QStandardItem *getSortKey() { return (mpSortKey); };

private:
    QStandardItem  *mpUrl;
    QStandardItem  *mpName;
    QStandardItem  *mpSortKey;
};
#endif // #ifndef A2BC_FAVORITE_BBS_DATA_H
/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
