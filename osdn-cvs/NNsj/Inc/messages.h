/*============================================================================*
 *  FILE: 
 *     messages.h
 *
 *  Description: 
 *     message symbols for NNsh.
 *
 *===========================================================================*/

#define MSG_USE_VFS_WARNING     "VFSは使用しません。"
#define MSG_CANNOT_LAUNCH       "領域の確保に失敗しました。起動できません。"
#define MSG_READFAIL_NNSH       "設定の読み出しに失敗しました。"
#define MSG_CANNOTFIND_BROWSER  "Webブラウザがインストールされていません。"
#define MSG_MESSAGE_DISCONNECT  "回線を切断します。"
#define MSG_DISCONNECT_WAIT     "回線切断中、お待ちください。"

#define MSG_TIMEOUT_UPDATED     "通信タイムアウト値を更新しました。\n"
#define MSG_ERROR_BUFFER_SIZE1  "入力されたバッファサイズ("
#define MSG_ERROR_BUFFER_SIZE2  ")が異常です。\n有効範囲："
#define MSG_ERROR_BUFFER_SIZE3  "～"
#define MSG_ERROR_BUFFER_SIZE4  "\n初期値："

#define MSG_SEND_TO_MEMOPAD     "マクロスクリプトをメモ帳に出力しました。"
#define MSG_DEVICEINFO_DATE      ""
#define MSG_DEVICEINFO_BATTERY   "バッテリ "
#define MSG_DEVICEINFO_VFS       "\nVFS(空き/最大)\n"
#define MSG_DEVICEINFO_MEM       "\nMEM(空き/最大)\n"

#define MSG_WRITE_OK             "編集データをマクロ設定しました\nDB名："
#define MSG_CONFIRM_APLEND       "アプリケーションを終了しますか？"
#define MSG_CONFIRM_OUTPUT_MACRO "現在の編集データをNNsiのマクロとして設定します。\nよろしいですか？"
#define MSG_CONFIRM_ALLDELETE    "編集データを破棄します。"
#define MSG_DELETELINE_POINT     "削除したい行を選択してください"
#define MSG_INSERTLINE_POINT     "挿入したい場所を選択してください"
#define MSG_EDITLINE_POINT       "編集したい行を選択してください"
#define MSG_CONFIRM_WRITE_DB     "編集データをDBに保存します。\nDB名："
#define MSG_CONFIRM_READ_DB      "編集データをDBから読み出します。\nDB名："
#define MSG_CONFIRM_DELETE_DB    " を削除します。(マクロDBかどうかのチェックを行わないため、同一名称の別DBを消去するかもしれません。)\nよろしいですか？"
#define MSG_WRITEDB_OK           "編集データをDBに保存しました。\nDB名："
#define MSG_DB_DELETED           "DBを削除しました。\nDB名："
