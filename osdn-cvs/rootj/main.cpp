/****************************************************************************
 ** $Id: main.cpp,v 1.1.1.1 2005/04/17 15:07:38 mrsa Exp $
 **
 **  Qtアプリケーションの起動処理 (Qt3用)
 ****************************************************************************/

/* インクルードファイルの設定 */
#include <qapplication.h>
#include "MainApp.h"

/* -----------------------------------------------------------------------**
 *   ...プロトタイプ宣言...
 * -----------------------------------------------------------------------**/

/* 引数の解析 */
int  parseArgument(int argc, char **argv, MainApp *aWinP);

/* メインウィンドウ表示前処理 */
void prepareWindow(MainApp *aWinP);

/***************************************************************************/
/**  メインの起動処理                                                     **/
/**                  （MainAppクラスを開く場合、この部分の変更は不要...） **/
/***************************************************************************/
int main(int argc, char **argv)
{
    /** アプリケーションのウィンドウ作成 **/
    QApplication apl(argc, argv);
    MainApp *mainWindow = new MainApp;

    /**  引数の解釈ロジック **/
    int result = parseArgument(argc, argv, mainWindow);
    if (result < 0)
    {
        // 引数の解析ﾉ失敗、、終了する
        return (result);
    }

    // メインウィンドウを設定する
    apl.setMainWidget(mainWindow);
    prepareWindow(mainWindow);

    // メインウィンドウの初期化処理
    mainWindow->initializeSelf();
    
    // メインWindowを表示する
    mainWindow->show();
    result = apl.exec();

    // メインウィンドウのfilnailze処理
    mainWindow->finalizeSelf();
    
    // メインWindowを削除する
    delete mainWindow;

    return (result);
}

/**************************************************************************
   引数の解析を行う (TODO : 解析を行い、MainAppに情報を渡す)
 **************************************************************************/
int parseArgument(int argc, char **argv, MainApp *aWinP)
{
/*****
    if ( argc == 2 && strcmp( argv[1], "-transparent" ) == 0 )
    {
        aWinP->setAutoMask( TRUE );
    }
    aWinP->resize( 100, 100 );
*****/

    return (0);
}

/**************************************************************************
   ウBンドウの表示前準備をsう (TODO : ウィンドウのタイトルを設定する)
 **************************************************************************/
void prepareWindow(MainApp *aWinP)
{
/**
    aWinP->setCaption("the Qt Application");
**/
    return;
}
