#ifndef A2BC_FAVORITE_BBS_EDITOR_H
#define A2BC_FAVORITE_BBS_EDITOR_H

#include <QWidget>
#include <QDialog>
#include "a2BCbbsTableTrim.h"
#include "ui_addBoard.h"

/*!
 *  a2BCfavoriteBbsEditor
 * 
 */
class screenData;
class QTableView;
class a2BCfavoriteBbsEditor : public QObject
{
    Q_OBJECT

public:
    a2BCfavoriteBbsEditor(QObject *parent = 0);
    ~a2BCfavoriteBbsEditor();

public:
    void prepare(screenData *dbData, QTableView *tableViewWidget);

signals:
    void updateMessage(const QString &);
    void savedBbsEditor();

public slots:
    void saveBbsTableInformations();
    void deleteBoardBbsInformation();
    void appendBoardBbsInformation();
    void pickupBoardBbsInformation();

public:
    bool readBbsTableInformations();
    void startBbsTableEditor();

private:


private:
    screenData        *mpDB;
    QTableView        *mpBbsTableTableWidget;
    a2BCbbsTableTrim   mBbsTableTrimmer;
};
#endif // #ifndef A2BC_FAVORITE_BBS_EDITOR_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
