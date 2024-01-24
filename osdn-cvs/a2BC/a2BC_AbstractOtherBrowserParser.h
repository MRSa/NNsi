/**
 *   他2ちゃんブラウザのデータクラスとの同期用インタフェースクラス
 * 
 */
#ifndef A2BC_ABSTRACTOTHERBROWSERPARSER_H
#define A2BC_ABSTRACTOTHERBROWSERPARSER_H
#include <QtGlobal>

class QString;
class bbsTableParser;
class a2BC_AbstractOtherBrowserParser
{
public:
    virtual ~a2BC_AbstractOtherBrowserParser() { };

public:
    /**
     *   ブラウザ名を応答する
     *   (ダイアログボックスや実行中ウィンドウに表示する文字列です)
     * 
     *   @return ブラウザ名 
     */
    virtual QString getBrowserName(void) = 0;

    /**
     *   ログを格納しているディレクトリ(のルートディレクトリ)を設定する
     *   (フォームの「管理ディレクトリ」に指定されている文字列が渡されます)
     *  @param arPath ログ管理ディレクトリ
     * 
     */
    virtual void setPath(QString &arPath) = 0;
    
    /**
     *   BBSTABLE.HTMLの解析情報を設定する
     *   
     *   @param apBbsTable 板一覧(URL)を持っているクラスへのポインタ
     *   @param arBbsTable bbstable.htmlへの絶対パス
     */
    virtual void setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable) = 0;
    
    /**
     *   クラス使用の準備を行う
     *   (ブラウザが管理しているdatファイルを認識する)
     * 
     *   @return  true:準備成功, false:準備失敗
     */
    virtual bool prepare(void) = 0;

    /**
     *  ブラウザが管理しているログデータ(datファイル)数を応答する
     * 
     *  @return ログデータ(datファイル)数
     */
    virtual int  datFileCount(void) = 0;

    /**
     *   ブラウザが管理していないが、存在しているログデータ(datファイル)を検出し、
     *  そのログデータ数を応答する
     *  （a2B用のみ...他ブラウザは 0 を応答する）
     * 
     *  @return ブラウザ管理外のログデータ数
     */
    virtual int  checkUnmanagedDatFile(void) = 0;

    /**
     *   ブラウザが管理していないログデータのファイル名を応答する 
     *   （a2B用のみ...他ブラウザは ""(空文字列)を応答する）
     * 
     *  @param aIndex インデックス番号
     *  @return インデックス番号にある、管理外のログファイル名
     */
    virtual QString getUnmanagedDatFileName(int aIndex) = 0;

    /**
     *   ブラウザが管理していないログデータの板ニックネーム名を取得する
     *   （a2B用のみ...他ブラウザは ""(空文字列)を応答する）
     * 
     *  @param aIndex インデックス番号
     *  @return インデックス番号にある、管理外のログの板ニックネーム
     * 
     */
    virtual QString getUnmanagedDatNickName(int aIndex) = 0;

    /**
     *   ブラウザが管理するログデータ(datファイル)のファイル名（フルパス）を生成する
     * （他ブラウザへ送る場合のファイル名）
     * 
     *  　@param arNick 板ニックネーム (mobile/ とか chakumelo/ とか)
     *    @param arFileName datファイル名 (12345678.dat とか、、、)
     *    @return ファイル名 (フルパス)
     */
    virtual QString getFullPathDatFileName(QString &arNick, QString &arFileName) = 0;

    /**
     *   ブラウザが管理するログデータ(datファイル)のファイル名（フルパス）を生成する
     *  （他ブラウザから取り込む場合のファイル名）
     * 
     *  　@param arNick 板ニックネーム (mobile/ とか chakumelo/ とか)
     *    @param arFileName datファイル名 (12345678.dat とかa2Bからくるときには、拡張子が違っていたりもする)
     * 
     */
    virtual QString getReceiveDatFileName(QString &arNick, QString &arFileName) = 0;

    /**
     *   インデックス番号にある、datファイル名を取得する
     * 
     *   @param aIndex インデックス番号
     *   @return datファイル名
     */
    virtual QString getDatFileName(int aIndex) = 0;

    /**
     *   インデックス番号にある、板ニックネームを取得する
     * 
     *   @param aIndex インデックス番号
     *   @return 板ニックネーム
     */
    virtual QString getBoardNick(int aIndex) = 0;

    /**
     *   ブラウザが管理しているレス区切りがa2B（2ちゃんねるサーバ）と同じかどうか？
     *   (0x0a なら同じ、0x0d, 0x0aなら違う)
     * 
     *   @return true:同じ(0x0a)  false:違う(0x0d, 0x0a)
     */
    virtual bool   isSameLogFormat(void) = 0;

    /**
     *  インデックス番号にある、読んでいる場所（レス番号）を取得する
     * 
     *   @param aIndex インデックス番号
     *   @return  読んでいる場所（「ここまで読んだ」レス番号）
     * 
     */
    virtual int    getReadPoint(int aIndex) = 0;

    /**
     *  ログを最後に更新した時間を取得する
     * 
     *   @param aIndex インデックス番号
     *   @return  最終更新時間 (管理していない場合には、0を応答する)
     * 
     */
    virtual qint64  getLastUpdate(int aIndex) = 0;

    /**
     *  ブラウザが管理している subject.txt のファイルについて、フルパスを取得する
     * （他ブラウザからsubject.txtをコピーするときに使用する）
     * 
     *  @param arNick 板ニックネーム
     *  @return 板ニックネームのsubject.txt（フルパス）
     * 
     */
    virtual QString getSubjectTxtFilePath(QString &arNick) = 0;

    /**
     *  datファイルをブラウザの管理下へ移動させる。
     * （元のブラウザにあるdatファイルを移動させる）
     * 
     *  @param arNick      板ニックネーム
     *  @param arFile      ファイル名
     *  @param arDst       移動先datファイル (full path)
     *  @param arSrc       移動元datファイル (full path)
     *  @param isSame      レス区切りが2ちゃんねるサーバ(a2B)と同じ形式か？
     *  @param aReadPoint  未読レス位置
     *  @param aLastUpdate 最終更新時間
     * 
     */
    virtual bool receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate) = 0;

    /**
     *  datファイルをブラウザの管理下へコピーする。
     * （元のブラウザにあるdatファイルをコピーする）
     * 
     *  @param arNick      板ニックネーム
     *  @param arFile      ファイル名
     *  @param arDst       コピー先datファイル (full path)
     *  @param arSrc       コピー元datファイル (full path)
     *  @param isSame      レス区切りが2ちゃんねるサーバ(a2B)と同じ形式か？
     *  @param aReadPoint  未読レス位置
     *  @param aLastUpdate 最終更新時間
     * 
     */
    virtual bool copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate) = 0;

    /**
     *  ログ同期のあとしまつ。
     * 
     *  @return true:後始末終了  false:失敗
     * 
     */
    virtual bool finalize(void) = 0;

};
#endif // #ifndef #ifndef A2BC_ABSTRACTOTHERBROWSERPARSER_H
/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
