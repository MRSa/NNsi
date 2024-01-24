/*!
 *   実処理
 *
 */
#include "screenControl.h"
#include "screenWidget.h"

/*!
 *  コンストラクタ
 */
screenWidget::screenWidget(QWidget *parent)
  : QWidget(parent)
{
  // do nothing!
}

/*!
 *  画面の準備...
 *
 */
bool screenWidget::prepare(QMainWindow *main, Ui::a2bInstaller *uiInstaller, screenControl *control)
{
    if (control->checkFilesExist() == false)
    {
        return (false);
    }
    control->prepare();
    main->show();

    return (true);
}
