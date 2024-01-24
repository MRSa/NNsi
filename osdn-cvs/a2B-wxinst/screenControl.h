/*!
 *   êßå‰ÉNÉâÉX
 *
 */
#ifndef SCREENCONTROL_H
#define SCREENCONTROL_H

#include <QWidget>
#include <QStatusBar>
#include "ui_wizard.h"
#include "screenData.h"

class screenControl : public QWidget
{
    Q_OBJECT

public:
    screenControl(Ui::a2bInstaller *apForm, QWidget *parent = 0);
    virtual ~screenControl() {};

public slots:
    void clicked_ButtonAbort();
    void clicked_ButtonBack();
    void clicked_ButtonNext();
    void clicked_detailButton();
    void clicked_EditText();

    void aboutQt();

public:
    bool checkFilesExist(void);
    void prepare(void);

private:
    void connectSignals(void);

private:
    Ui::a2bInstaller *mpForm;
    screenData        mDB;
    int               currentScene;
};

#endif
