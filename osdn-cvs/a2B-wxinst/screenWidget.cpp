/*!
 *   ������
 *
 */
#include "screenControl.h"
#include "screenWidget.h"

/*!
 *  �R���X�g���N�^
 */
screenWidget::screenWidget(QWidget *parent)
  : QWidget(parent)
{
  // do nothing!
}

/*!
 *  ��ʂ̏���...
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
