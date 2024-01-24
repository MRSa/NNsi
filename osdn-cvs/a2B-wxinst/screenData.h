/*!
 *   é¿èàóùÉNÉâÉX
 *
 */
#ifndef SCREENDATA_H
#define SCREENDATA_H

#include <QWidget>
#include <QString>
#include "ui_wizard.h"

class screenData : public QWidget
{
    Q_OBJECT

public:
    screenData(QWidget *parent = 0);
    virtual ~screenData() {};

public:
    void prepare(Ui::a2bInstaller *apForm);

public:
    void doStart(void);
    void doAbort(void);
    void doNext(void);
    void doBack(void);
    void setDetail(void);
    void editTextFinished(void);

private:
    void updateScreen(void);
    void setStartScreen(void);
    void setSpecifyDirectory(void);
    void setConfirmCopy(void);
    void setEndScreen(void);
    bool executeCopyFiles(void);

private:
    Ui::a2bInstaller *wizardForm;
    int               currentScene;
    QString           logDirectory;
    QString           copyDirectory;
};

#endif


