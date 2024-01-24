/*
 *   画面制御部..
 *
 */
#include <QtGlobal>
#include "mainControl.h"

/*
 *  コンストラクタ (画面表示準備)
 *
 */
mainControl::mainControl(QWidget *parent) : QWidget(parent)
{
    // ここでは何もしない
}

/*
 *  デストラクタ (画面表示のあとしまつ)
 *
 */
mainControl::~mainControl()
{
    // ここでは何もしない
}

/*
 *   メイン画面の表示準備
 *
 */
void mainControl::setupMainUi(QMainWindow *apWidget)
{
    // メインWidgetを設定してからシグナルとスロットを接続する
    prepareObjects();
    mpMainForm->setupUi(apWidget);
    prepareDatas();
    connectSignals();

    // データの展開
    startup();

    return;
}

/*
 *  翻訳ファイル名の取得
 *
 */
QString mainControl::getTranslateFileName(void)
{
    /*
     *  ここで翻訳ファイル名を指定する
     *
     */
    return ("qTVg_jp");
}

/*
 *  起動処理
 *
 */
void mainControl::startApp(int argc, char *argv[])
{
    // アプリケーションの開始処理...
    mpMainProcedures->startApp();
    return;
}

/*
 *  終了処理
 *
 */
void mainControl::finishApp(void)
{
    // アプリケーションの終了処理...
    mpMainProcedures->finishApp();
    return;
}

/*
 *  オブジェクトの生成
 *
 */
void mainControl::prepareObjects(void)
{
    mpMainForm       = new Ui::mainForm();
    mpMainProcedures = new mainProcedures(mpMainForm);
    return;
}

/*
 *  シグナルとスロットの接続
 *
 */
void mainControl::connectSignals(void)
{
    // About Qtの表示
    connect(mpMainForm->actionAbout_Qt, SIGNAL(triggered(bool)),
            mpMainProcedures,           SLOT(show_aboutQt(bool)));

    // バージョン情報の表示
    connect(mpMainForm->actionShow_Version, SIGNAL(triggered(bool)),
            mpMainProcedures,               SLOT(show_version(bool)));

    // ログの取得実行
    connect(mpMainForm->btnGet, SIGNAL(clicked()),
            mpMainProcedures,   SLOT(getDataFile()));

    // ログファイルの削除
    connect(mpMainForm->btnClear, SIGNAL(clicked()),
            mpMainProcedures,     SLOT(deleteDataFile()));

    // ログ記録ディレクトリの指定
    connect(mpMainForm->btnSelectDir, SIGNAL(clicked()),
            mpMainProcedures,         SLOT(selectDirectory()));

    // カレンダーウィジェットが更新されたとき...
    connect(mpMainForm->btnToday,       SIGNAL(clicked()),
            mpMainProcedures,           SLOT(moveToToday()));
    connect(mpMainForm->calendarWidget, SIGNAL(clicked()),
            mpMainProcedures,           SLOT(updateLogInformation()));
    connect(mpMainForm->calendarWidget, SIGNAL(activated()),
            mpMainProcedures,           SLOT(updateLogInformation()));
    connect(mpMainForm->calendarWidget, SIGNAL(selectionChanged()),
            mpMainProcedures,           SLOT(updateLogInformation()));

    return;
}


/*
 *  データの設定（地域データを設定する）
 *
 */
void mainControl::prepareDatas(void)
{
    // 地域選択のコンボボックスにアイテムを挿入する
    (mpMainForm->cmbAreaId)->addItem(tr("Sapporo"),     QVariant(1));
    (mpMainForm->cmbAreaId)->addItem(tr("Hakodate"),    QVariant(52));
    (mpMainForm->cmbAreaId)->addItem(tr("Asahikawa"),   QVariant(48));
    (mpMainForm->cmbAreaId)->addItem(tr("Obihiro"),     QVariant(50));
    (mpMainForm->cmbAreaId)->addItem(tr("Kushiro"),     QVariant(51));
    (mpMainForm->cmbAreaId)->addItem(tr("Kitami"),      QVariant(49));
    (mpMainForm->cmbAreaId)->addItem(tr("Muroran"),     QVariant(53));
    (mpMainForm->cmbAreaId)->addItem(tr("Aomori"),      QVariant(2));
    (mpMainForm->cmbAreaId)->addItem(tr("Iwate"),       QVariant(3));
    (mpMainForm->cmbAreaId)->addItem(tr("Miyagi"),      QVariant(4));
    (mpMainForm->cmbAreaId)->addItem(tr("Akita"),       QVariant(5));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamagata"),    QVariant(6));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukushima"),   QVariant(7));
    (mpMainForm->cmbAreaId)->addItem(tr("Ibaraki"),     QVariant(8));
    (mpMainForm->cmbAreaId)->addItem(tr("Tochigi"),     QVariant(9));
    (mpMainForm->cmbAreaId)->addItem(tr("Gunma"),       QVariant(10));
    (mpMainForm->cmbAreaId)->addItem(tr("Saitama"),     QVariant(11));
    (mpMainForm->cmbAreaId)->addItem(tr("Chiba"),       QVariant(12));
    (mpMainForm->cmbAreaId)->addItem(tr("Tokyo"),       QVariant(13));
    (mpMainForm->cmbAreaId)->addItem(tr("Kanagawa"),    QVariant(14));
    (mpMainForm->cmbAreaId)->addItem(tr("Niigata"),     QVariant(15));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamanashi"),   QVariant(16));
    (mpMainForm->cmbAreaId)->addItem(tr("Nagano"),      QVariant(17));
    (mpMainForm->cmbAreaId)->addItem(tr("Toyama"),      QVariant(18));
    (mpMainForm->cmbAreaId)->addItem(tr("Ishikawa"),    QVariant(19));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukui"),       QVariant(20));
    (mpMainForm->cmbAreaId)->addItem(tr("Gifu"),        QVariant(21));
    (mpMainForm->cmbAreaId)->addItem(tr("Shizuoka"),    QVariant(22));
    (mpMainForm->cmbAreaId)->addItem(tr("Aichi"),       QVariant(23));
    (mpMainForm->cmbAreaId)->addItem(tr("Mie"),         QVariant(24));
    (mpMainForm->cmbAreaId)->addItem(tr("Shiga"),       QVariant(25));
    (mpMainForm->cmbAreaId)->addItem(tr("Kyoto"),       QVariant(26));
    (mpMainForm->cmbAreaId)->addItem(tr("Osaka"),       QVariant(27));
    (mpMainForm->cmbAreaId)->addItem(tr("Hyogo"),       QVariant(28));
    (mpMainForm->cmbAreaId)->addItem(tr("Nara"),        QVariant(29));
    (mpMainForm->cmbAreaId)->addItem(tr("Wakayama"),    QVariant(30));
    (mpMainForm->cmbAreaId)->addItem(tr("Tottori"),     QVariant(31));
    (mpMainForm->cmbAreaId)->addItem(tr("Shimane"),     QVariant(32));
    (mpMainForm->cmbAreaId)->addItem(tr("Okayama"),     QVariant(33));
    (mpMainForm->cmbAreaId)->addItem(tr("Hiroshima"),   QVariant(34));
    (mpMainForm->cmbAreaId)->addItem(tr("Yamaguchi"),   QVariant(35));
    (mpMainForm->cmbAreaId)->addItem(tr("Tokushima"),   QVariant(36));
    (mpMainForm->cmbAreaId)->addItem(tr("Kagawa"),      QVariant(37));
    (mpMainForm->cmbAreaId)->addItem(tr("Ehime"),       QVariant(38));
    (mpMainForm->cmbAreaId)->addItem(tr("Kochi"),       QVariant(39));
    (mpMainForm->cmbAreaId)->addItem(tr("Fukuoka"),     QVariant(40));
    (mpMainForm->cmbAreaId)->addItem(tr("Kitakyusyu"),  QVariant(54));
    (mpMainForm->cmbAreaId)->addItem(tr("Saga"),        QVariant(41));
    (mpMainForm->cmbAreaId)->addItem(tr("Nagasaki"),    QVariant(42));
    (mpMainForm->cmbAreaId)->addItem(tr("Kumamoto"),    QVariant(43));
    (mpMainForm->cmbAreaId)->addItem(tr("Ooita"),       QVariant(44));
    (mpMainForm->cmbAreaId)->addItem(tr("Miyazaki"),    QVariant(45));
    (mpMainForm->cmbAreaId)->addItem(tr("Kagoshima"),   QVariant(46));
    (mpMainForm->cmbAreaId)->addItem(tr("Okinawa"),     QVariant(47));

    // 地域情報にチェックを入れる
    (mpMainForm->checkBox_0)->setCheckState(Qt::Checked);
    (mpMainForm->checkBox_1)->setCheckState(Qt::Checked);
    (mpMainForm->checkBox_2)->setCheckState(Qt::Checked);

    return;
}

/**
 *  処理の開始...
 */
void mainControl::startup(void)
{
    mpMainProcedures->startup();
    return;
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
